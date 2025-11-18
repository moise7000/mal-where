// Anti-debug utilities (C++03, MSVC 2010+ / MinGW compatible)
// Features:
//  - Detect debugger via IsDebuggerPresent() / CheckRemoteDebuggerPresent()
//  - Detect debugger via PEB (Process Environment Block)
//  - Detect timing attacks (RDTSC, GetTickCount)
//  - Check parent process name (detect OllyDbg, x64dbg, WinDbg)
//  - Compute checksum (Adler-32) of the running executable
//
// Windows-only (tested on Windows 7+)

#ifdef _WIN32

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <winternl.h>  // For PEB structure

#pragma comment(lib, "psapi.lib")

// Define intrinsics for GCC if not available
#ifdef __GNUC__
#ifndef __readfsdword
static inline unsigned long __readfsdword(unsigned long Offset) {
    unsigned long value;
    __asm__ __volatile__ ("movl %%fs:%1, %0" : "=r" (value) : "m" (*(unsigned long*)Offset));
    return value;
}
#endif
#ifndef __readgsqword
static inline unsigned long long __readgsqword(unsigned long Offset) {
    unsigned long long value;
    __asm__ __volatile__ ("movq %%gs:%1, %0" : "=r" (value) : "m" (*(unsigned long long*)Offset));
    return value;
}
#endif
#endif

namespace anti_debug {

// ========== Adler-32 checksum ==========
static unsigned long adler32_update(unsigned long adler, const unsigned char* data, size_t len) {
    const unsigned long MOD_ADLER = 65521UL;
    unsigned long a = adler & 0xFFFFUL;
    unsigned long b = (adler >> 16) & 0xFFFFUL;
    for (size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    return (b << 16) | a;
}

static unsigned long adler32_file(const wchar_t* path, unsigned long* out_size) {
    if (out_size) *out_size = 0;
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 1UL;
    
    unsigned long adler = 1UL;
    unsigned char buf[4096];
    DWORD bytesRead;
    
    while (ReadFile(hFile, buf, sizeof(buf), &bytesRead, NULL) && bytesRead > 0) {
        adler = adler32_update(adler, buf, bytesRead);
        if (out_size) *out_size += bytesRead;
    }
    
    CloseHandle(hFile);
    return adler;
}

// ========== Debugger detection ==========

// Method 1: IsDebuggerPresent() API
static bool is_debugger_present_api() {
    return IsDebuggerPresent() != 0;
}

// Method 2: CheckRemoteDebuggerPresent()
static bool is_debugger_present_remote() {
    BOOL isDebuggerPresent = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent);
    return isDebuggerPresent != 0;
}

// Method 3: PEB (Process Environment Block) - BeingDebugged flag
static bool is_debugger_present_peb() {
    // Use NtCurrentTeb() to access PEB (compatible with both MSVC and GCC)
    // This is a Windows intrinsic that works on all compilers
#ifdef _WIN64
    // x64: PEB is at offset 0x60 in TEB, BeingDebugged at offset 0x02 in PEB
    PPEB peb = (PPEB)__readgsqword(0x60);
    return peb->BeingDebugged != 0;
#else
    // x86: PEB is at offset 0x30 in TEB, BeingDebugged at offset 0x02 in PEB
    PPEB peb = (PPEB)__readfsdword(0x30);
    return peb->BeingDebugged != 0;
#endif
}

// Method 4: NtGlobalFlag in PEB
static bool is_debugger_present_ntglobalflag() {
    // Use intrinsics instead of inline assembly (compatible with both MSVC and GCC)
#ifdef _WIN64
    // x64: NtGlobalFlag is at offset 0xBC in PEB
    PPEB peb = (PPEB)__readgsqword(0x60);
    DWORD ntGlobalFlag = *(DWORD*)((BYTE*)peb + 0xBC);
#else
    // x86: NtGlobalFlag is at offset 0x68 in PEB
    PPEB peb = (PPEB)__readfsdword(0x30);
    DWORD ntGlobalFlag = *(DWORD*)((BYTE*)peb + 0x68);
#endif
    // FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS
    return (ntGlobalFlag & 0x70) != 0;
}

// ========== Timing attacks ==========

// RDTSC timing check
static bool detect_timing_rdtsc() {
    // Use __rdtsc intrinsic (works on both MSVC and GCC/MinGW)
    unsigned long long start = __rdtsc();
    
    // Some dummy operation
    volatile int x = 0;
    for (int i = 0; i < 100; i++) x++;
    
    unsigned long long end = __rdtsc();
    
    // If difference is too large, might be stepping through debugger
    unsigned long long diff = end - start;
    return diff > 10000; // Threshold
}

// GetTickCount timing check
static bool detect_timing_gettickcount() {
    DWORD start = GetTickCount();
    // Some dummy operation
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) x++;
    DWORD end = GetTickCount();
    
    // Should be < 10ms
    return (end - start) > 100;
}

// ========== Parent process check ==========

static bool parent_process_suspicious() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);
    
    DWORD currentPid = GetCurrentProcessId();
    DWORD parentPid = 0;
    
    // Find current process to get parent PID
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == currentPid) {
                parentPid = pe32.th32ParentProcessID;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    if (parentPid == 0) {
        CloseHandle(hSnapshot);
        return false;
    }
    
    // Find parent process name
    pe32.dwSize = sizeof(pe32);
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == parentPid) {
                // Check for suspicious debugger names
                wchar_t* name = pe32.szExeFile;
                _wcslwr(name);
                
                if (wcsstr(name, L"ollydbg") || 
                    wcsstr(name, L"x64dbg") || 
                    wcsstr(name, L"x32dbg") ||
                    wcsstr(name, L"windbg") ||
                    wcsstr(name, L"ida") ||
                    wcsstr(name, L"immunitydebugger") ||
                    wcsstr(name, L"devenv")) { // Visual Studio
                    CloseHandle(hSnapshot);
                    return true;
                }
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return false;
}

// ========== Checksum functions ==========

static unsigned long checksum_self(unsigned long* out_size) {
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
        if (out_size) *out_size = 0;
        return 1UL;
    }
    return adler32_file(exePath, out_size);
}

static unsigned long text_segments_checksum(unsigned long* out_size) {
    if (out_size) *out_size = 0;
    
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) return 1UL;
    
    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo)))
        return 1UL;
    
    unsigned char* base = (unsigned char*)modInfo.lpBaseOfDll;
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
    
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return 1UL;
    
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        return 1UL;
    
    IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(ntHeaders);
    unsigned long adler = 1UL;
    unsigned long totalSize = 0;
    
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++) {
        // Check if section is executable (IMAGE_SCN_MEM_EXECUTE)
        if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
            unsigned char* sectionData = base + section->VirtualAddress;
            unsigned long sectionSize = section->Misc.VirtualSize;
            
            adler = adler32_update(adler, sectionData, sectionSize);
            totalSize += sectionSize;
        }
    }
    
    if (out_size) *out_size = totalSize;
    return adler;
}

// ========== Environment checks ==========

static bool has_preload() {
    // Windows equivalent: Check for AppInit_DLLs registry key
    // or loaded suspicious DLLs
    return false; // Simplified for now
}

static bool env_suspicious() {
    // Check for common debugging environment variables
    const char* vars[] = { "_NT_SYMBOL_PATH", "_NT_ALT_SYMBOL_PATH", "COR_ENABLE_PROFILING", NULL };
    for (const char** v = vars; *v; ++v) {
        if (getenv(*v)) return true;
    }
    return false;
}

static bool loaded_objects_suspicious() {
    // Enumerate loaded modules and check for suspicious names
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    
    MODULEENTRY32W me32;
    me32.dwSize = sizeof(me32);
    
    bool suspicious = false;
    if (Module32FirstW(hSnapshot, &me32)) {
        do {
            wchar_t* name = me32.szModule;
            _wcslwr(name);
            
            if (wcsstr(name, L"frida") ||
                wcsstr(name, L"x64dbg") ||
                wcsstr(name, L"ollydbg") ||
                wcsstr(name, L"minhook") ||
                wcsstr(name, L"detours")) {
                suspicious = true;
                break;
            }
        } while (Module32NextW(hSnapshot, &me32));
    }
    
    CloseHandle(hSnapshot);
    return suspicious;
}

// ========== Public API ==========

bool is_debugger_present() {
    return is_debugger_present_api() || 
           is_debugger_present_remote() || 
           is_debugger_present_peb();
}

bool is_debugger_present_ptrace() {
    // Windows: use NtGlobalFlag instead
    return is_debugger_present_ntglobalflag();
}

} // namespace anti_debug

#else
// Non-Windows platform
#error "This version requires Windows. Use the Linux version for Linux systems."
#endif // _WIN32
