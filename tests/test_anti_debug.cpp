// Test anti-debug utilities for Windows
// Compatible with Windows 7+ (MSVC 2010+ / MinGW)

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#error "This test requires Windows"
#endif

// Include the anti_debug implementation
#include "../obfuscation_methods/anti_debug/anti_debug.h"
#include "../obfuscation_methods/anti_debug/anti_debug.cpp"

static void print_bool(const char* label, bool v) {
    std::cout << label << ": " << (v ? "YES" : "NO") << std::endl;
}

static void print_separator(const char* title) {
    std::cout << "\n========== " << title << " ==========" << std::endl;
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "     Anti-Debug Detection Suite - Windows 7+     " << std::endl;
    std::cout << "==================================================" << std::endl;

    // ========== Debugger Detection ==========
    print_separator("Debugger Detection");
    
    bool dbg = anti_debug::is_debugger_present();
    print_bool("IsDebuggerPresent() / PEB", dbg);
    
    bool dbg_nt = anti_debug::is_debugger_present_ptrace();
    print_bool("NtGlobalFlag (heap debug)", dbg_nt);
    
    if (dbg || dbg_nt) {
        std::cout << "\n*** DEBUGGER DETECTED ***" << std::endl;
        std::cout << "Running under: OllyDbg, x64dbg, WinDbg, or Visual Studio" << std::endl;
    } else {
        std::cout << "\nNo debugger detected (normal execution)" << std::endl;
    }

    // ========== Parent Process Check ==========
    print_separator("Parent Process Check");
    
    bool parent_sus = anti_debug::parent_process_suspicious();
    print_bool("Parent process suspicious", parent_sus);
    
    if (parent_sus) {
        std::cout << "Launched from a debugger!" << std::endl;
    }

    // ========== Environment Variables ==========
    print_separator("Environment Variables");
    
    bool env_sus = anti_debug::env_suspicious();
    print_bool("Env suspicious (_NT_*)", env_sus);
    
    // Test with setting a variable
    _putenv("_NT_SYMBOL_PATH=C:\\symbols");
    bool env_sus2 = anti_debug::env_suspicious();
    print_bool("After _NT_SYMBOL_PATH set", env_sus2);
    
    _putenv("_NT_SYMBOL_PATH=");
    bool env_sus3 = anti_debug::env_suspicious();
    print_bool("After _NT_SYMBOL_PATH unset", env_sus3);

    // ========== Loaded Modules ==========
    print_separator("Loaded Modules");
    
    bool loaded_sus = anti_debug::loaded_objects_suspicious();
    print_bool("Suspicious DLLs loaded", loaded_sus);
    
    if (loaded_sus) {
        std::cout << "Detected: Frida, x64dbg plugins, MinHook, or Detours" << std::endl;
    }

    // ========== Preload Check ==========
    print_separator("Preload Check");
    
    bool preload = anti_debug::has_preload();
    print_bool("Has preload (Windows)", preload);
    std::cout << "(Note: Always false on Windows, simplified)" << std::endl;

    // ========== Binary Checksums ==========
    print_separator("Binary Integrity");
    
    unsigned long size1 = 0, size2 = 0;
    unsigned long chk1 = anti_debug::checksum_self(&size1);
    unsigned long chk2 = anti_debug::checksum_self(&size2);
    
    std::cout << "Executable checksum #1: 0x" << std::hex << chk1 
              << std::dec << " (" << size1 << " bytes)" << std::endl;
    std::cout << "Executable checksum #2: 0x" << std::hex << chk2 
              << std::dec << " (" << size2 << " bytes)" << std::endl;
    
    if (chk1 == chk2 && size1 == size2) {
        std::cout << "[OK] Checksum stable" << std::endl;
    } else {
        std::cout << "[FAIL] Checksum mismatch!" << std::endl;
    }

    // ========== Text Segments Checksum ==========
    print_separator("Code Sections Integrity");
    
    unsigned long text_size = 0;
    unsigned long text_chk = anti_debug::text_segments_checksum(&text_size);
    std::cout << "Text sections checksum: 0x" << std::hex << text_chk 
              << std::dec << " (" << text_size << " bytes)" << std::endl;
    
    // Load a DLL and check if text checksum changes
    std::cout << "\nLoading kernel32.dll (should not affect our .text)..." << std::endl;
    HMODULE hKernel = LoadLibraryA("kernel32.dll");
    
    unsigned long text_size2 = 0;
    unsigned long text_chk2 = anti_debug::text_segments_checksum(&text_size2);
    std::cout << "After LoadLibrary: 0x" << std::hex << text_chk2 
              << std::dec << " (" << text_size2 << " bytes)" << std::endl;
    
    if (text_chk == text_chk2) {
        std::cout << "[OK] Text sections unchanged" << std::endl;
    } else {
        std::cout << "[WARNING] Text sections changed (possible breakpoints or patches)" << std::endl;
    }
    
    if (hKernel) FreeLibrary(hKernel);

    // ========== Summary ==========
    print_separator("Summary");
    
    int total_detections = 0;
    if (dbg) total_detections++;
    if (dbg_nt) total_detections++;
    if (parent_sus) total_detections++;
    if (env_sus) total_detections++;
    if (loaded_sus) total_detections++;
    
    std::cout << "Total detections: " << total_detections << "/5" << std::endl;
    
    if (total_detections > 0) {
        std::cout << "\n*** WARNING: Debugging/instrumentation detected! ***" << std::endl;
        std::cout << "This program is being analyzed." << std::endl;
    } else {
        std::cout << "\n[OK] No suspicious activity detected" << std::endl;
        std::cout << "Program is running in a clean environment." << std::endl;
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "                   Test Complete                  " << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
