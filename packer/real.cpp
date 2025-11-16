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
    uint32_t key[4];         // Clé de déchiffrement
    // Le payload suit immédiatement après
};
#pragma pack(pop)

// ==================== CODE DU STUB UNPACKER (sera compilé séparément) ====================
// Ce code sera le stub d'extraction injecté dans l'exe packed

const char* stubSourceCode = R"(
#include <windows.h>
#include <stdio.h>

#pragma pack(push, 1)
struct PackedSection {
    DWORD magic;
    DWORD unpacked_size;
    DWORD packed_size;
    DWORD key[4];
};
#pragma pack(pop)

void decryptXOR(unsigned char* data, DWORD size, DWORD* key) {
    unsigned char* keyBytes = (unsigned char*)key;
    for (DWORD i = 0; i < size; i++) {
        data[i] ^= keyBytes[i % 16];
    }
}

DWORD decompressRLE(unsigned char* input, DWORD inputSize, unsigned char* output, DWORD outputSize) {
    DWORD writePos = 0;
    DWORD readPos = 0;

    while (readPos < inputSize && writePos < outputSize) {
        unsigned char current = input[readPos];

        if (current == 0xFF && readPos + 2 < inputSize) {
            unsigned char count = input[readPos + 1];
            if (count == 0) {
                output[writePos++] = 0xFF;
                readPos += 2;
            } else {
                unsigned char value = input[readPos + 2];
                for (int i = 0; i < count && writePos < outputSize; i++) {
                    output[writePos++] = value;
                }
                readPos += 3;
            }
        } else {
            output[writePos++] = current;
            readPos++;
        }
    }

    return writePos;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Obtenir le chemin de l'exécutable actuel
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // Ouvrir l'exécutable pour lire la section packed
    HANDLE hFile = CreateFileA(exePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 1;

    // Mapper le fichier en mémoire
    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping) {
        CloseHandle(hFile);
        return 1;
    }

    unsigned char* fileData = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!fileData) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    // Trouver la section .packed
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)fileData;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(fileData + dosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);

    PackedSection* packedSec = NULL;
    unsigned char* packedData = NULL;

    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        if (memcmp(sections[i].Name, ".packed", 7) == 0) {
            packedSec = (PackedSection*)(fileData + sections[i].PointerToRawData);
            packedData = (unsigned char*)packedSec + sizeof(PackedSection);
            break;
        }
    }

    if (!packedSec || packedSec->magic != 0x4B435041) {
        UnmapViewOfFile(fileData);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    // Allouer buffers
    unsigned char* decrypted = (unsigned char*)VirtualAlloc(NULL, packedSec->packed_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    unsigned char* decompressed = (unsigned char*)VirtualAlloc(NULL, packedSec->unpacked_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!decrypted || !decompressed) {
        UnmapViewOfFile(fileData);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    // Copier et déchiffrer
    memcpy(decrypted, packedData, packedSec->packed_size);
    decryptXOR(decrypted, packedSec->packed_size, packedSec->key);

    // Décompresser
    DWORD decompSize = decompressRLE(decrypted, packedSec->packed_size, decompressed, packedSec->unpacked_size);

    // Créer fichier temporaire
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    GetTempFileNameA(tempPath, "tmp", 0, tempFile);

    // Changer l'extension en .exe
    char* ext = strrchr(tempFile, '.');
    if (ext) strcpy(ext, ".exe");

    // Écrire le fichier décompressé
    HANDLE hTempFile = CreateFileA(tempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (hTempFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hTempFile, decompressed, decompSize, &written, NULL);
        CloseHandle(hTempFile);

        // Nettoyer les ressources
        VirtualFree(decrypted, 0, MEM_RELEASE);
        VirtualFree(decompressed, 0, MEM_RELEASE);
        UnmapViewOfFile(fileData);
        CloseHandle(hMapping);
        CloseHandle(hFile);

        // Exécuter le fichier temporaire
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);

        if (CreateProcessA(tempFile, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        // Supprimer le fichier temporaire
        DeleteFileA(tempFile);
    }

    return 0;
}
)";

class SimplePacker {
private:
    std::vector<BYTE> inputFile;
    std::string inputPath;
    std::string outputPath;
    DWORD fileSize;
    bool lockFlag;
    uint32_t lockKey[4];
    uint32_t lockHash;

    // ==================== COMPILATION DU STUB ====================
    bool compileStub(const std::string& stubExePath) {
        // Créer un fichier source temporaire
        char tempPath[MAX_PATH];
        char stubSourcePath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubSourcePath, "%s\\stub_source.c", tempPath);

        // Écrire le code source
        FILE* f = fopen(stubSourcePath, "w");
        if (!f) {
            fprintf(stderr, "[-] Could not create stub source file\n");
            return false;
        }
        fprintf(f, "%s", stubSourceCode);
        fclose(f);

        // Compiler avec GCC
        char compileCmd[1024];
        sprintf(compileCmd, "gcc -O2 -s -mwindows -o \"%s\" \"%s\" 2>nul",
                stubExePath.c_str(), stubSourcePath);

        printf("[*] Compiling unpacker stub...\n");
        int result = system(compileCmd);

        // Nettoyer
        DeleteFileA(stubSourcePath);

        if (result != 0) {
            fprintf(stderr, "[-] Failed to compile stub. Make sure GCC is installed.\n");
            fprintf(stderr, "[-] You can install MinGW or use: winget install -e --id GnuWin32.Make\n");
            return false;
        }

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

    // ==================== HASH DJB2 ====================
    static uint32_t hashDJB2(const BYTE* buf, size_t size) {
        uint32_t hash = 5381;
        for (size_t i = 0; i < size; i++) {
            hash = ((hash << 5) + hash) + buf[i];
        }
        return hash;
    }

    static uint32_t hashString(const std::string& str) {
        return hashDJB2(reinterpret_cast<const BYTE*>(str.c_str()), str.length());
    }

    static DWORD alignValue(DWORD value, DWORD alignment) {
        DWORD r = value % alignment;
        return r ? value + (alignment - r) : value;
    }

    static std::string intToString(int value) {
        char buffer[32];
        sprintf(buffer, "%d", value);
        return std::string(buffer);
    }

    // ==================== INJECTION DES DONNÉES DANS LE STUB ====================
    bool injectPackedData(const std::string& stubExePath,
                          const std::vector<BYTE>& packedData,
                          const std::string& outputPath) {
        // Lire le stub compilé
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

        // Analyser le PE du stub
        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&stubData[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&stubData[0] + dosHeader->e_lfanew);
        IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders);

        // Trouver la dernière section
        IMAGE_SECTION_HEADER* lastSection = &sections[ntHeaders->FileHeader.NumberOfSections - 1];

        // Créer la nouvelle section .packed
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

        // Mettre à jour le nombre de sections
        ntHeaders->FileHeader.NumberOfSections++;

        // Mettre à jour SizeOfImage
        ntHeaders->OptionalHeader.SizeOfImage = alignValue(newSection.VirtualAddress + newSection.Misc.VirtualSize,
                                                           ntHeaders->OptionalHeader.SectionAlignment);

        // Créer le fichier final
        std::vector<BYTE> finalData;

        // Copier jusqu'à la table des sections
        size_t sectionsOffset = dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS);
        finalData.insert(finalData.end(), stubData.begin(),
                        stubData.begin() + sectionsOffset + (ntHeaders->FileHeader.NumberOfSections - 1) * sizeof(IMAGE_SECTION_HEADER));

        // Ajouter la nouvelle section header
        finalData.insert(finalData.end(), reinterpret_cast<BYTE*>(&newSection),
                        reinterpret_cast<BYTE*>(&newSection) + sizeof(newSection));

        // Copier le reste jusqu'aux données
        size_t afterSections = sectionsOffset + ntHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
        finalData.insert(finalData.end(), stubData.begin() + afterSections, stubData.end());

        // Padding jusqu'à la nouvelle section
        while (finalData.size() < newSection.PointerToRawData) {
            finalData.push_back(0);
        }

        // Ajouter les données packed
        finalData.insert(finalData.end(), packedData.begin(), packedData.end());

        // Padding final
        while (finalData.size() % ntHeaders->OptionalHeader.FileAlignment != 0) {
            finalData.push_back(0);
        }

        // Écrire le fichier final
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
    SimplePacker() : fileSize(0), lockFlag(false), lockHash(0) {
        memset(lockKey, 0, sizeof(lockKey));
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

        // Compression
        printf("[*] Compressing with RLE...\n");
        std::vector<BYTE> compressed = compressRLE(inputFile);
        printf("[+] Compressed: %lu bytes (%.1f%%)\n",
               (unsigned long)compressed.size(),
               (100.0 * compressed.size() / fileSize));

        // Clé de chiffrement
        uint32_t defaultKey[4] = { 0x12345678, 0x9ABCDEF0, 0xFEDCBA98, 0x87654321 };

        printf("[*] Encrypting payload...\n");
        encryptXOR(compressed, defaultKey);
        printf("[+] Encryption complete\n");

        // Création de la section packed
        std::vector<BYTE> packedSectionData(sizeof(PackedSection) + compressed.size());
        PackedSection* section = reinterpret_cast<PackedSection*>(&packedSectionData[0]);

        section->magic = 0x4B435041; // "PACK"
        section->unpacked_size = fileSize;
        section->packed_size = static_cast<DWORD>(compressed.size());
        memcpy(section->key, defaultKey, sizeof(defaultKey));

        memcpy(&packedSectionData[0] + sizeof(PackedSection),
               &compressed[0], compressed.size());

        // Compiler le stub
        char tempPath[MAX_PATH];
        char stubExePath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        sprintf(stubExePath, "%s\\unpacker_stub.exe", tempPath);

        if (!compileStub(stubExePath)) {
            return false;
        }

        // Injecter les données dans le stub
        printf("[*] Injecting packed data into stub...\n");
        if (!injectPackedData(stubExePath, packedSectionData, outputPath)) {
            DeleteFileA(stubExePath);
            return false;
        }

        // Nettoyer le stub temporaire
        DeleteFileA(stubExePath);

        printf("\n[+] ========================================\n");
        printf("[+] Successfully packed!\n");
        printf("[+] Original size:  %lu bytes\n", (unsigned long)fileSize);
        printf("[+] Packed size:    Unknown (check file)\n");
        printf("[+] Compression:    %.1f%%\n", (100.0 * compressed.size() / fileSize));
        printf("[+] Output file:    %s\n", outputPath.c_str());
        printf("[+] ========================================\n\n");
        printf("[i] The packed exe will:\n");
        printf("    1. Extract to %%TEMP%%\\tmpXXXX.exe\n");
        printf("    2. Execute the original program\n");
        printf("    3. Delete the temp file after execution\n\n");

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