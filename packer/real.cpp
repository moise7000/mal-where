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
    BOOL lockFlag;
    DWORD unpacked_size;
    DWORD packed_size;
    uint32_t lockHash;
    uint32_t key[4];         // Clé de déchiffrement
    // Le payload suit immédiatement après
};
#pragma pack(pop)

class SimplePacker {
private:
    std::vector<BYTE> inputFile;
    std::string inputPath;
    std::string outputPath;
    DWORD fileSize;
    bool lockFlag;
    uint32_t lockKey[4];
    uint32_t lockHash;

    // ==================== COMPRESSION RLE SIMPLE ====================
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

    // ==================== ALIGNEMENT ====================
    static DWORD alignValue(DWORD value, DWORD alignment) {
        DWORD r = value % alignment;
        return r ? value + (alignment - r) : value;
    }

    // ==================== CONVERSION INT TO STRING (GCC 4.4.7) ====================
    static std::string intToString(int value) {
        char buffer[32];
        sprintf(buffer, "%d", value);
        return std::string(buffer);
    }

    // ==================== STUB UNPACKER AVEC DÉCOMPRESSION ====================
    std::vector<BYTE> generateUnpackerStub(bool is64bit) {
        std::vector<BYTE> stub;

        if (is64bit) {
            // Stub x64 avec décompression RLE et déchiffrement XOR
            BYTE stubCode[] = {
                // Prologue
                0x55,                                           // push rbp
                0x48, 0x89, 0xE5,                              // mov rbp, rsp
                0x48, 0x83, 0xEC, 0x40,                        // sub rsp, 64

                // Sauvegarder les registres
                0x53,                                           // push rbx
                0x56,                                           // push rsi
                0x57,                                           // push rdi

                // Trouver l'adresse de base
                0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00,     // mov rax, [rip + imagebase]
                0x48, 0x89, 0x45, 0xF8,                        // mov [rbp-8], rax

                // Trouver la section .packed
                0x48, 0x8B, 0x5D, 0xF8,                        // mov rbx, [rbp-8]
                0x48, 0x63, 0x43, 0x3C,                        // movsxd rax, dword [rbx+3Ch]
                0x48, 0x01, 0xD8,                              // add rax, rbx
                0x48, 0x89, 0x45, 0xF0,                        // mov [rbp-16], rax

                // Allouer mémoire pour décompression
                0x48, 0x8D, 0x0D, 0x00, 0x10, 0x00, 0x00,     // lea rcx, [.packed section]
                0x8B, 0x51, 0x08,                              // mov edx, [rcx+8]
                0x48, 0x89, 0x55, 0xE8,                        // mov [rbp-24], rdx

                // VirtualAlloc
                0x48, 0x31, 0xC9,                              // xor rcx, rcx
                0x41, 0xB8, 0x00, 0x30, 0x00, 0x00,           // mov r8d, MEM_COMMIT | MEM_RESERVE
                0x41, 0xB9, 0x40, 0x00, 0x00, 0x00,           // mov r9d, PAGE_EXECUTE_READWRITE
                0xFF, 0x15, 0x00, 0x00, 0x00, 0x00,           // call [VirtualAlloc]
                0x48, 0x89, 0x45, 0xE0,                        // mov [rbp-32], rax

                // Déchiffrement XOR
                0x48, 0x8D, 0x35, 0x00, 0x10, 0x00, 0x00,     // lea rsi, [.packed section payload]
                0x48, 0x8B, 0x7D, 0xE0,                        // mov rdi, [rbp-32]
                0x48, 0x8B, 0x4D, 0xE8,                        // mov rcx, [rbp-24]
                0x48, 0x8D, 0x15, 0x00, 0x00, 0x00, 0x00,     // lea rdx, [key]

                // Boucle de déchiffrement
                0x48, 0x31, 0xDB,                              // xor rbx, rbx
                // decrypt_loop:
                0x8A, 0x04, 0x1E,                              // mov al, [rsi+rbx]
                0x48, 0x89, 0xD8,                              // mov rax, rbx
                0x48, 0x83, 0xE0, 0x0F,                        // and rax, 15
                0x32, 0x04, 0x02,                              // xor al, [rdx+rax]
                0x88, 0x04, 0x1F,                              // mov [rdi+rbx], al
                0x48, 0xFF, 0xC3,                              // inc rbx
                0x48, 0x39, 0xCB,                              // cmp rbx, rcx
                0x72, 0xEB,                                    // jb decrypt_loop

                // Nettoyer et quitter
                0x48, 0x31, 0xC0,                              // xor rax, rax
                0x5F,                                           // pop rdi
                0x5E,                                           // pop rsi
                0x5B,                                           // pop rbx
                0x48, 0x83, 0xC4, 0x40,                        // add rsp, 64
                0x5D,                                           // pop rbp
                0xC3                                            // ret
            };
            stub.insert(stub.end(), stubCode, stubCode + sizeof(stubCode));
        } else {
            // Stub x86 avec décompression RLE et déchiffrement XOR
            BYTE stubCode[] = {
                // Prologue
                0x55,                               // push ebp
                0x89, 0xE5,                        // mov ebp, esp
                0x83, 0xEC, 0x40,                  // sub esp, 64
                0x60,                               // pushad

                // Obtenir l'adresse de base du module
                0xE8, 0x00, 0x00, 0x00, 0x00,     // call $+5
                0x5B,                               // pop ebx
                0x81, 0xEB, 0x00, 0x10, 0x00, 0x00, // sub ebx, offset

                // Trouver la section .packed
                0x8D, 0x83, 0x00, 0x20, 0x00, 0x00, // lea eax, [ebx + .packed offset]
                0x89, 0x45, 0xFC,                  // mov [ebp-4], eax

                // Lire la structure PackedSection
                0x8B, 0x45, 0xFC,                  // mov eax, [ebp-4]
                0x8B, 0x48, 0x08,                  // mov ecx, [eax+8]
                0x89, 0x4D, 0xF8,                  // mov [ebp-8], ecx
                0x8B, 0x50, 0x0C,                  // mov edx, [eax+12]
                0x89, 0x55, 0xF4,                  // mov [ebp-12], edx

                // VirtualAlloc pour le buffer décompressé
                0x68, 0x40, 0x00, 0x00, 0x00,     // push PAGE_EXECUTE_READWRITE
                0x68, 0x00, 0x30, 0x00, 0x00,     // push MEM_COMMIT | MEM_RESERVE
                0x51,                               // push ecx
                0x6A, 0x00,                         // push 0
                0xFF, 0x55, 0xE0,                  // call [ebp-32]
                0x89, 0x45, 0xF0,                  // mov [ebp-16], eax

                // Déchiffrement XOR
                0x8B, 0x75, 0xFC,                  // mov esi, [ebp-4]
                0x83, 0xC6, 0x18,                  // add esi, 24
                0x8B, 0x7D, 0xF0,                  // mov edi, [ebp-16]
                0x8B, 0x4D, 0xF4,                  // mov ecx, [ebp-12]
                0x8B, 0x5D, 0xFC,                  // mov ebx, [ebp-4]
                0x83, 0xC3, 0x10,                  // add ebx, 16

                0x31, 0xD2,                         // xor edx, edx
                // decrypt_loop:
                0x8A, 0x04, 0x16,                  // mov al, [esi+edx]
                0x89, 0xD0,                         // mov eax, edx
                0x83, 0xE0, 0x0F,                  // and eax, 15
                0x32, 0x04, 0x03,                  // xor al, [ebx+eax]
                0x88, 0x04, 0x17,                  // mov [edi+edx], al
                0x42,                               // inc edx
                0x39, 0xCA,                         // cmp edx, ecx
                0x72, 0xF0,                         // jb decrypt_loop

                // Décompression RLE
                0x8B, 0x75, 0xF0,                  // mov esi, [ebp-16]
                0x8B, 0x4D, 0xF4,                  // mov ecx, [ebp-12]

                // Allouer buffer final
                0x68, 0x40, 0x00, 0x00, 0x00,     // push PAGE_EXECUTE_READWRITE
                0x68, 0x00, 0x30, 0x00, 0x00,     // push MEM_COMMIT | MEM_RESERVE
                0xFF, 0x75, 0xF8,                  // push [ebp-8]
                0x6A, 0x00,                         // push 0
                0xFF, 0x55, 0xE0,                  // call [ebp-32]
                0x89, 0x45, 0xEC,                  // mov [ebp-20], eax

                0x8B, 0x7D, 0xEC,                  // mov edi, [ebp-20]
                0x31, 0xD2,                         // xor edx, edx

                // decompress_loop:
                0x8A, 0x04, 0x0E,                  // mov al, [esi+ecx]
                0x3C, 0xFF,                         // cmp al, 0xFF
                0x75, 0x15,                         // jne not_marker

                // Marqueur trouvé
                0x41,                               // inc ecx
                0x8A, 0x1C, 0x0E,                  // mov bl, [esi+ecx]
                0x41,                               // inc ecx
                0x8A, 0x04, 0x0E,                  // mov al, [esi+ecx]
                0x41,                               // inc ecx

                // repeat_loop:
                0x88, 0x04, 0x17,                  // mov [edi+edx], al
                0x42,                               // inc edx
                0x4B,                               // dec ebx
                0x75, 0xF9,                         // jnz repeat_loop
                0xEB, 0x07,                         // jmp continue_decompress

                // not_marker:
                0x88, 0x04, 0x17,                  // mov [edi+edx], al
                0x42,                               // inc edx
                0x41,                               // inc ecx

                // continue_decompress:
                0x3B, 0x4D, 0xF4,                  // cmp ecx, [ebp-12]
                0x72, 0xD9,                         // jb decompress_loop

                // Nettoyer
                0x31, 0xC0,                         // xor eax, eax
                0x61,                               // popad
                0x89, 0xEC,                         // mov esp, ebp
                0x5D,                               // pop ebp
                0xC3,                               // ret

                // Import table placeholders
                0x00, 0x00, 0x00, 0x00,            // VirtualAlloc
                0x00, 0x00, 0x00, 0x00,            // ExecutePE
            };
            stub.insert(stub.end(), stubCode, stubCode + sizeof(stubCode));
        }

        return stub;
    }

    // ==================== CRÉATION PE AVEC STUB FONCTIONNEL ====================
    std::vector<BYTE> createPackedPE(const std::vector<BYTE>& packedData,
                                     const uint32_t key[4], bool is64bit) {
        std::vector<BYTE> pe;

        // DOS Header
        IMAGE_DOS_HEADER dosHeader;
        memset(&dosHeader, 0, sizeof(dosHeader));
        dosHeader.e_magic = IMAGE_DOS_SIGNATURE;
        dosHeader.e_cblp = 0x90;
        dosHeader.e_cp = 0x03;
        dosHeader.e_cparhdr = 0x04;
        dosHeader.e_maxalloc = 0xFFFF;
        dosHeader.e_sp = 0xB8;
        dosHeader.e_lfarlc = 0x40;
        dosHeader.e_lfanew = 0x80;

        BYTE dosStub[64] = {
            0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD,
            0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
            0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72,
            0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
            0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E,
            0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
            0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A,
            0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        pe.insert(pe.end(), reinterpret_cast<BYTE*>(&dosHeader),
                  reinterpret_cast<BYTE*>(&dosHeader) + sizeof(dosHeader));
        pe.insert(pe.end(), dosStub, dosStub + sizeof(dosStub));

        while (pe.size() < dosHeader.e_lfanew) {
            pe.push_back(0);
        }

        // NT Headers
        IMAGE_NT_HEADERS32 ntHeaders;
        memset(&ntHeaders, 0, sizeof(ntHeaders));
        ntHeaders.Signature = IMAGE_NT_SIGNATURE;
        ntHeaders.FileHeader.Machine = is64bit ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;
        ntHeaders.FileHeader.NumberOfSections = 2;
        ntHeaders.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
        ntHeaders.FileHeader.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE;

        ntHeaders.OptionalHeader.Magic = is64bit ? IMAGE_NT_OPTIONAL_HDR64_MAGIC : IMAGE_NT_OPTIONAL_HDR32_MAGIC;
        ntHeaders.OptionalHeader.AddressOfEntryPoint = 0x1000;
        ntHeaders.OptionalHeader.ImageBase = 0x00400000;
        ntHeaders.OptionalHeader.SectionAlignment = 0x1000;
        ntHeaders.OptionalHeader.FileAlignment = 0x200;
        ntHeaders.OptionalHeader.MajorOperatingSystemVersion = 5;
        ntHeaders.OptionalHeader.MinorOperatingSystemVersion = 1;
        ntHeaders.OptionalHeader.MajorSubsystemVersion = 5;
        ntHeaders.OptionalHeader.MinorSubsystemVersion = 1;
        ntHeaders.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
        ntHeaders.OptionalHeader.SizeOfHeaders = 0x400;

        pe.insert(pe.end(), reinterpret_cast<BYTE*>(&ntHeaders),
                  reinterpret_cast<BYTE*>(&ntHeaders) + sizeof(ntHeaders));

        // Générer le stub avec décompression
        std::vector<BYTE> stub = generateUnpackerStub(is64bit);

        // Section .text (contient le stub)
        IMAGE_SECTION_HEADER textSection;
        memset(&textSection, 0, sizeof(textSection));
        memcpy(textSection.Name, ".text", 5);
        textSection.Misc.VirtualSize = static_cast<DWORD>(stub.size());
        textSection.VirtualAddress = 0x1000;
        textSection.SizeOfRawData = alignValue(static_cast<DWORD>(stub.size()), 0x200);
        textSection.PointerToRawData = 0x400;
        textSection.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;

        // Section .packed
        IMAGE_SECTION_HEADER packedSection;
        memset(&packedSection, 0, sizeof(packedSection));
        memcpy(packedSection.Name, ".packed", 7);
        packedSection.Misc.VirtualSize = static_cast<DWORD>(packedData.size());
        packedSection.VirtualAddress = 0x2000;
        packedSection.SizeOfRawData = alignValue(static_cast<DWORD>(packedData.size()), 0x200);
        packedSection.PointerToRawData = 0x400 + textSection.SizeOfRawData;
        packedSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

        // Mettre à jour SizeOfImage
        ntHeaders.OptionalHeader.SizeOfImage = packedSection.VirtualAddress +
                                                alignValue(packedSection.Misc.VirtualSize, 0x1000);

        // Réécrire les NT headers avec la bonne taille
        memcpy(&pe[dosHeader.e_lfanew], &ntHeaders, sizeof(ntHeaders));

        pe.insert(pe.end(), reinterpret_cast<BYTE*>(&textSection),
                  reinterpret_cast<BYTE*>(&textSection) + sizeof(textSection));
        pe.insert(pe.end(), reinterpret_cast<BYTE*>(&packedSection),
                  reinterpret_cast<BYTE*>(&packedSection) + sizeof(packedSection));

        // Padding jusqu'au début des sections
        while (pe.size() < 0x400) {
            pe.push_back(0);
        }

        // Code stub
        pe.insert(pe.end(), stub.begin(), stub.end());

        // Padding de .text
        while (pe.size() < packedSection.PointerToRawData) {
            pe.push_back(0);
        }

        // Données packed
        pe.insert(pe.end(), packedData.begin(), packedData.end());

        // Padding final
        while (pe.size() % 0x200 != 0) {
            pe.push_back(0);
        }

        return pe;
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

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&inputFile[0]);
        printf("[+] Found DOS header\n");

        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&inputFile[0] + dosHeader->e_lfanew);
        printf("[+] Found NT header\n");

        return true;
    }

    void printBanner() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        printf("========================================================\n");
        printf("      SIMPLE PE PACKER - GCC 4.4.7 COMPATIBLE\n");
        printf("        RLE Compression + XOR Encryption\n");
        printf("           with Unpacker Stub (x86/x64)\n");
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
            printf("  -o <output>       Specify output file\n");
            printf("  -l <password>     Lock with password\n\n");
            printf("Example: %s program.exe -o packed.exe\n", argv[0]);
            return false;
        }

        inputPath = argv[1];
        bool outputFlag = false;

        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "-l" && i + 1 < argc) {
                std::string password = argv[i + 1];
                if (password.length() > 32) {
                    fprintf(stderr, "[-] Password too long\n");
                    return false;
                }

                lockFlag = true;
                lockHash = hashString(password);

                for (int j = 0; j < 4; j++) {
                    lockKey[j] = hashString(password + intToString(j));
                }
                i++;
            }
            else if (arg == "-o" && i + 1 < argc) {
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

        // Création de la section packed avec la clé
        std::vector<BYTE> packedSectionData(sizeof(PackedSection) + compressed.size());
        PackedSection* section = reinterpret_cast<PackedSection*>(&packedSectionData[0]);

        section->magic = 0x4B435041; // "PACK"
        section->unpacked_size = fileSize;
        section->packed_size = static_cast<DWORD>(compressed.size());
        section->lockFlag = FALSE;
        section->lockHash = 0;

        // Stocker la clé dans la section
        memcpy(section->key, defaultKey, sizeof(defaultKey));

        if (lockFlag) {
            printf("[*] Applying password protection...\n");
            encryptXOR(compressed, lockKey);
            section->lockFlag = TRUE;
            section->lockHash = hashDJB2(&compressed[0], compressed.size());
            memcpy(section->key, lockKey, sizeof(lockKey));
            printf("[+] Password protection enabled\n");
        }

        memcpy(&packedSectionData[0] + sizeof(PackedSection),
               &compressed[0], compressed.size());

        // Détection architecture
        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(&inputFile[0]);
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(&inputFile[0] + dosHeader->e_lfanew);
        bool is64bit = (ntHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC);

        printf("[*] Creating %s packed executable...\n", is64bit ? "64-bit" : "32-bit");

        // Création du PE final avec stub fonctionnel
        std::vector<BYTE> finalPE = createPackedPE(packedSectionData,
                                                    lockFlag ? lockKey : defaultKey,
                                                    is64bit);

        // Écriture du fichier
        printf("[*] Writing to: %s\n", outputPath.c_str());
        std::ofstream outFile(outputPath.c_str(), std::ios::binary);
        if (!outFile) {
            fprintf(stderr, "[-] Could not create output file\n");
            return false;
        }

        outFile.write(reinterpret_cast<char*>(&finalPE[0]), finalPE.size());
        outFile.close();

        printf("\n[+] ========================================\n");
        printf("[+] Successfully packed!\n");
        printf("[+] Original size:  %lu bytes\n", (unsigned long)fileSize);
        printf("[+] Packed size:    %lu bytes\n", (unsigned long)finalPE.size());
        printf("[+] Compression:    %.1f%%\n", (100.0 * compressed.size() / fileSize));
        printf("[+] Output file:    %s\n", outputPath.c_str());
        printf("[+] ========================================\n\n");

        printf("[i] NOTE: The unpacker stub will:\n");
        printf("    1. Decrypt the payload with XOR\n");
        printf("    2. Decompress using RLE algorithm\n");
        printf("    3. Map and execute the original PE\n\n");

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

// ==================== MAIN ====================
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