#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// Structure pour la section packed
#pragma pack(push, 1)
struct PackedSection {
    DWORD magic;             // Signature 0x4B435041 ("PACK")
    DWORD unpacked_size;
    DWORD packed_size;
    DWORD key[4];            // Clé de déchiffrement
    // Le payload suit immédiatement après
};
#pragma pack(pop)

// ==================== CODE DU STUB UNPACKER ====================
// Générer le code source du stub dans un fichier
void generateStubSource(const char* outputPath) {
    FILE* f = fopen(outputPath, "w");
    if (!f) return;

    fprintf(f, "#include <windows.h>\n");
    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "#pragma pack(push, 1)\n");
    fprintf(f, "struct PackedSection {\n");
    fprintf(f, "    DWORD magic;\n");
    fprintf(f, "    DWORD unpacked_size;\n");
    fprintf(f, "    DWORD packed_size;\n");
    fprintf(f, "    DWORD key[4];\n");
    fprintf(f, "};\n");
    fprintf(f, "#pragma pack(pop)\n\n");

    fprintf(f, "void decryptXOR(unsigned char* data, DWORD size, DWORD* key) {\n");
    fprintf(f, "    unsigned char* keyBytes = (unsigned char*)key;\n");
    fprintf(f, "    DWORD i;\n");
    fprintf(f, "    for (i = 0; i < size; i++) {\n");
    fprintf(f, "        data[i] ^= keyBytes[i %% 16];\n");
    fprintf(f, "    }\n");
    fprintf(f, "}\n\n");

    fprintf(f, "DWORD decompressRLE(unsigned char* input, DWORD inputSize, unsigned char* output, DWORD outputSize) {\n");
    fprintf(f, "    DWORD writePos = 0;\n");
    fprintf(f, "    DWORD readPos = 0;\n");
    fprintf(f, "    int i;\n");
    fprintf(f, "    while (readPos < inputSize && writePos < outputSize) {\n");
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

    // Utiliser int main au lieu de WinMain pour avoir la console
    fprintf(f, "int main(int argc, char* argv[]) {\n");
    fprintf(f, "    char exePath[MAX_PATH];\n");
    fprintf(f, "    GetModuleFileNameA(NULL, exePath, MAX_PATH);\n\n");

    fprintf(f, "    HANDLE hFile = CreateFileA(exePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);\n");
    fprintf(f, "    if (hFile == INVALID_HANDLE_VALUE) {\n");
    fprintf(f, "        printf(\"Error: Cannot open self\\n\");\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);\n");
    fprintf(f, "    if (!hMapping) {\n");
    fprintf(f, "        printf(\"Error: Cannot map file\\n\");\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    unsigned char* fileData = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);\n");
    fprintf(f, "    if (!fileData) {\n");
    fprintf(f, "        printf(\"Error: Cannot map view\\n\");\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)fileData;\n");
    fprintf(f, "    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(fileData + dosHeader->e_lfanew);\n");
    fprintf(f, "    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);\n\n");

    fprintf(f, "    struct PackedSection* packedSec = NULL;\n");
    fprintf(f, "    unsigned char* packedData = NULL;\n");
    fprintf(f, "    int i;\n\n");

    fprintf(f, "    for (i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {\n");
    fprintf(f, "        if (memcmp(sections[i].Name, \".packed\", 7) == 0) {\n");
    fprintf(f, "            packedSec = (struct PackedSection*)(fileData + sections[i].PointerToRawData);\n");
    fprintf(f, "            packedData = (unsigned char*)packedSec + sizeof(struct PackedSection);\n");
    fprintf(f, "            break;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    if (!packedSec) {\n");
    fprintf(f, "        printf(\"Error: .packed section not found\\n\");\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    if (packedSec->magic != 0x4B435041) {\n");
    fprintf(f, "        printf(\"Error: Invalid magic (0x%%08X)\\n\", packedSec->magic);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    unsigned char* decrypted = (unsigned char*)VirtualAlloc(NULL, packedSec->packed_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n");
    fprintf(f, "    unsigned char* decompressed = (unsigned char*)VirtualAlloc(NULL, packedSec->unpacked_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);\n\n");

    fprintf(f, "    if (!decrypted || !decompressed) {\n");
    fprintf(f, "        printf(\"Error: Memory allocation failed\\n\");\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    memcpy(decrypted, packedData, packedSec->packed_size);\n");
    fprintf(f, "    decryptXOR(decrypted, packedSec->packed_size, packedSec->key);\n");
    fprintf(f, "    DWORD decompSize = decompressRLE(decrypted, packedSec->packed_size, decompressed, packedSec->unpacked_size);\n\n");

    fprintf(f, "    if (decompSize == 0 || decompSize > packedSec->unpacked_size) {\n");
    fprintf(f, "        printf(\"Error: Decompression failed (size=%%lu)\\n\", decompSize);\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    char tempPath[MAX_PATH];\n");
    fprintf(f, "    char tempFile[MAX_PATH];\n");
    fprintf(f, "    GetTempPathA(MAX_PATH, tempPath);\n");
    fprintf(f, "    GetTempFileNameA(tempPath, \"tmp\", 0, tempFile);\n\n");

    fprintf(f, "    char* ext = strrchr(tempFile, '.');\n");
    fprintf(f, "    if (ext) strcpy(ext, \".exe\");\n\n");

    fprintf(f, "    HANDLE hTempFile = CreateFileA(tempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);\n");
    fprintf(f, "    if (hTempFile == INVALID_HANDLE_VALUE) {\n");
    fprintf(f, "        printf(\"Error: Cannot create temp file: %%s\\n\", tempFile);\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    DWORD written;\n");
    fprintf(f, "    BOOL writeOk = WriteFile(hTempFile, decompressed, decompSize, &written, NULL);\n");
    fprintf(f, "    CloseHandle(hTempFile);\n\n");

    fprintf(f, "    if (!writeOk || written != decompSize) {\n");
    fprintf(f, "        printf(\"Error: Write failed (%%lu / %%lu bytes)\\n\", written, decompSize);\n");
    fprintf(f, "        DeleteFileA(tempFile);\n");
    fprintf(f, "        VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "        VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "        UnmapViewOfFile(fileData);\n");
    fprintf(f, "        CloseHandle(hMapping);\n");
    fprintf(f, "        CloseHandle(hFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    VirtualFree(decrypted, 0, MEM_RELEASE);\n");
    fprintf(f, "    VirtualFree(decompressed, 0, MEM_RELEASE);\n");
    fprintf(f, "    UnmapViewOfFile(fileData);\n");
    fprintf(f, "    CloseHandle(hMapping);\n");
    fprintf(f, "    CloseHandle(hFile);\n\n");

    // Construire la ligne de commande avec tous les arguments
    fprintf(f, "    char cmdLine[32768] = {0};\n");
    fprintf(f, "    if (argc > 1) {\n");
    fprintf(f, "        for (i = 1; i < argc; i++) {\n");
    fprintf(f, "            if (i > 1) strcat(cmdLine, \" \");\n");
    fprintf(f, "            strcat(cmdLine, argv[i]);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    STARTUPINFOA si;\n");
    fprintf(f, "    PROCESS_INFORMATION pi;\n");
    fprintf(f, "    memset(&si, 0, sizeof(si));\n");
    fprintf(f, "    memset(&pi, 0, sizeof(pi));\n");
    fprintf(f, "    si.cb = sizeof(si);\n\n");

    fprintf(f, "    BOOL created = CreateProcessA(tempFile, cmdLine[0] ? cmdLine : NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);\n");
    fprintf(f, "    if (!created) {\n");
    fprintf(f, "        printf(\"Error: Cannot start process (error %%lu)\\n\", GetLastError());\n");
    fprintf(f, "        DeleteFileA(tempFile);\n");
    fprintf(f, "        return 1;\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    WaitForSingleObject(pi.hProcess, INFINITE);\n");
    fprintf(f, "    DWORD exitCode = 0;\n");
    fprintf(f, "    GetExitCodeProcess(pi.hProcess, &exitCode);\n");
    fprintf(f, "    CloseHandle(pi.hProcess);\n");
    fprintf(f, "    CloseHandle(pi.hThread);\n");
    fprintf(f, "    DeleteFileA(tempFile);\n\n");

    fprintf(f, "    return exitCode;\n");
    fprintf(f, "}\n");

    fclose(f);
}

class SimplePacker {
private:
    std::vector<BYTE> inputFile;
    std::string inputPath;
    std::string outputPath;
    DWORD fileSize;

    // ==================== COMPILATION DU STUB ====================
    bool compileStub(const std::string& stubExePath) {
        char tempPath[MAX_PATH];
        char stubSourcePath[MAX_PATH];
        char errorLogPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubSourcePath, "%s\\stub_source.c", tempPath);
        sprintf(errorLogPath, "%s\\stub_compile_error.txt", tempPath);

        printf("[*] Generating stub source code...\n");
        printf("    Source: %s\n", stubSourcePath);
        generateStubSource(stubSourcePath);

        // Vérifier que le fichier a été créé
        HANDLE hTest = CreateFileA(stubSourcePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hTest == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "[-] Failed to create stub source file\n");
            return false;
        }
        CloseHandle(hTest);

        char compileCmd[1024];
        // Compiler sans -mwindows pour avoir une application console
        sprintf(compileCmd, "gcc -O2 -s -o \"%s\" \"%s\" 2>\"%s\"",
                stubExePath.c_str(), stubSourcePath, errorLogPath);

        printf("[*] Compiling unpacker stub...\n");
        printf("    Command: %s\n", compileCmd);
        int result = system(compileCmd);

        // Vérifier si le stub a été créé
        HANDLE hStub = CreateFileA(stubExePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        bool stubExists = (hStub != INVALID_HANDLE_VALUE);
        if (stubExists) CloseHandle(hStub);

        if (result != 0 || !stubExists) {
            fprintf(stderr, "[-] Failed to compile stub (exit code: %d)\n", result);

            // Afficher les erreurs de compilation
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

            fprintf(stderr, "[-] Stub source saved at: %s\n", stubSourcePath);
            fprintf(stderr, "[-] Try compiling manually: gcc -mwindows -o stub.exe \"%s\"\n", stubSourcePath);

            DeleteFileA(errorLogPath);
            // Ne pas supprimer le source pour debug
            // DeleteFileA(stubSourcePath);
            return false;
        }

        // Nettoyer les fichiers temporaires
        DeleteFileA(stubSourcePath);
        DeleteFileA(errorLogPath);

        printf("[+] Stub compiled successfully\n");
        return true;
    }

    // ==================== COMPRESSION RLE ====================
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

    // ==================== CHIFFREMENT XOR ====================
    void encryptXOR(std::vector<BYTE>& data, const uint32_t key[4]) {
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

    // ==================== INJECTION DES DONNÉES ====================
    bool injectPackedData(const std::string& stubExePath,
                          const std::vector<BYTE>& packedData,
                          const std::string& outputPath) {
        std::ifstream stubFile(stubExePath.c_str(), std::ios::binary | std::ios::ate);
        if (!stubFile) {
            fprintf(stderr, "[-] Could not open stub file\n");
            return false;
        }

        size_t stubSize = stubFile.tellg();
        stubFile.seekg(0, std::ios::beg);

        std::vector<BYTE> stubData(stubSize);
        stubFile.read(reinterpret_cast<char*>(&stubData[0]), stubSize);
        stubFile.close();

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&stubData[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&stubData[0] + dosHeader->e_lfanew);
        IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);

        IMAGE_SECTION_HEADER* lastSection = &sections[ntHeaders->FileHeader.NumberOfSections - 1];

        IMAGE_SECTION_HEADER newSection;
        memset(&newSection, 0, sizeof(newSection));
        memcpy(newSection.Name, ".packed", 7);
        newSection.Misc.VirtualSize = static_cast<DWORD>(packedData.size());
        newSection.VirtualAddress = alignValue(lastSection->VirtualAddress + lastSection->Misc.VirtualSize,
                                              ntHeaders->OptionalHeader.SectionAlignment);
        newSection.SizeOfRawData = alignValue(static_cast<DWORD>(packedData.size()),
                                             ntHeaders->OptionalHeader.FileAlignment);
        newSection.PointerToRawData = alignValue(lastSection->PointerToRawData + lastSection->SizeOfRawData,
                                                ntHeaders->OptionalHeader.FileAlignment);
        newSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

        ntHeaders->FileHeader.NumberOfSections++;
        ntHeaders->OptionalHeader.SizeOfImage = alignValue(newSection.VirtualAddress + newSection.Misc.VirtualSize,
                                                           ntHeaders->OptionalHeader.SectionAlignment);

        std::vector<BYTE> finalData;
        size_t sectionsOffset = dosHeader->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + ntHeaders->FileHeader.SizeOfOptionalHeader;

        finalData.insert(finalData.end(), stubData.begin(),
                        stubData.begin() + sectionsOffset + (ntHeaders->FileHeader.NumberOfSections - 1) * sizeof(IMAGE_SECTION_HEADER));

        finalData.insert(finalData.end(), reinterpret_cast<BYTE*>(&newSection),
                        reinterpret_cast<BYTE*>(&newSection) + sizeof(newSection));

        size_t afterSections = sectionsOffset + ntHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
        if (afterSections < stubData.size()) {
            finalData.insert(finalData.end(), stubData.begin() + afterSections, stubData.end());
        }

        while (finalData.size() < newSection.PointerToRawData) {
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

        fileSize = static_cast<DWORD>(file.tellg());
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

        printf("[+] Found valid PE file\n");
        return true;
    }

    void printBanner() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        printf("========================================================\n");
        printf("    PE PACKER - TEMP FILE EXTRACTION METHOD\n");
        printf("      RLE Compression + XOR Encryption\n");
        printf("       Fully Functional on Windows 7+\n");
        printf("========================================================\n\n");

        SetConsoleTextAttribute(hConsole, 7);
    }

public:
    SimplePacker() : fileSize(0) {}

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

        printf("[*] Compressing with RLE...\n");
        std::vector<BYTE> compressed = compressRLE(inputFile);
        printf("[+] Compressed: %lu bytes (%.1f%%)\n",
               (unsigned long)compressed.size(),
               (100.0 * compressed.size() / fileSize));

        uint32_t defaultKey[4] = { 0x12345678, 0x9ABCDEF0, 0xFEDCBA98, 0x87654321 };

        printf("[*] Encrypting payload...\n");
        encryptXOR(compressed, defaultKey);
        printf("[+] Encryption complete\n");

        std::vector<BYTE> packedSectionData(sizeof(PackedSection) + compressed.size());
        PackedSection* section = reinterpret_cast<PackedSection*>(&packedSectionData[0]);

        section->magic = 0x4B435041;
        section->unpacked_size = fileSize;
        section->packed_size = static_cast<DWORD>(compressed.size());
        memcpy(section->key, defaultKey, sizeof(defaultKey));

        memcpy(&packedSectionData[0] + sizeof(PackedSection),
               &compressed[0], compressed.size());

        char tempPath[MAX_PATH];
        char stubExePath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubExePath, "%s\\unpacker_stub.exe", tempPath);

        if (!compileStub(stubExePath)) {
            return false;
        }

        printf("[*] Injecting packed data into stub...\n");
        if (!injectPackedData(stubExePath, packedSectionData, outputPath)) {
            DeleteFileA(stubExePath);
            return false;
        }

        DeleteFileA(stubExePath);

        printf("\n[+] ========================================\n");
        printf("[+] Successfully packed!\n");
        printf("[+] Original size:  %lu bytes\n", (unsigned long)fileSize);
        printf("[+] Compression:    %.1f%%\n", (100.0 * compressed.size() / fileSize));
        printf("[+] Output file:    %s\n", outputPath.c_str());
        printf("[+] ========================================\n\n");
        printf("[i] The packed exe will extract and run the original.\n\n");

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
        SimplePacker packer;
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