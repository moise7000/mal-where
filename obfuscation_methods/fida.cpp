//
// Created by ewan decima on 11/15/25.
//

// make_exe.cpp - Génère un .exe Windows avec chaîne de jumps
#include <stdint.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

#ifndef INT32_MIN
#define INT32_MIN (-2147483647 - 1)
#endif
#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif

// Structure PE minimale
struct DOSHeader {
    uint16_t e_magic;      // "MZ"
    uint8_t  padding[58];
    uint32_t e_lfanew;     // Offset vers PE header
};

struct PEHeader {
    uint32_t signature;    // "PE\0\0"
    // COFF Header
    uint16_t machine;      // 0x8664 pour x64
    uint16_t numberOfSections;
    uint32_t timeDateStamp;
    uint32_t pointerToSymbolTable;
    uint32_t numberOfSymbols;
    uint16_t sizeOfOptionalHeader;
    uint16_t characteristics;
};

struct OptionalHeader64 {
    uint16_t magic;        // 0x20B pour PE32+
    uint8_t  majorLinkerVersion;
    uint8_t  minorLinkerVersion;
    uint32_t sizeOfCode;
    uint32_t sizeOfInitializedData;
    uint32_t sizeOfUninitializedData;
    uint32_t addressOfEntryPoint;
    uint32_t baseOfCode;
    uint64_t imageBase;
    uint32_t sectionAlignment;
    uint32_t fileAlignment;
    uint16_t majorOperatingSystemVersion;
    uint16_t minorOperatingSystemVersion;
    uint16_t majorImageVersion;
    uint16_t minorImageVersion;
    uint16_t majorSubsystemVersion;
    uint16_t minorSubsystemVersion;
    uint32_t win32VersionValue;
    uint32_t sizeOfImage;
    uint32_t sizeOfHeaders;
    uint32_t checkSum;
    uint16_t subsystem;
    uint16_t dllCharacteristics;
    uint64_t sizeOfStackReserve;
    uint64_t sizeOfStackCommit;
    uint64_t sizeOfHeapReserve;
    uint64_t sizeOfHeapCommit;
    uint32_t loaderFlags;
    uint32_t numberOfRvaAndSizes;
    // Data directories (16 entries de 8 bytes chacune)
    uint8_t dataDirectories[16 * 8];
};

struct SectionHeader {
    char     name[8];
    uint32_t virtualSize;
    uint32_t virtualAddress;
    uint32_t sizeOfRawData;
    uint32_t pointerToRawData;
    uint32_t pointerToRelocations;
    uint32_t pointerToLinenumbers;
    uint16_t numberOfRelocations;
    uint16_t numberOfLinenumbers;
    uint32_t characteristics;
};

int main(int argc, char** argv){
    int N = 2000;

    if(argc >= 2) N = std::atoi(argv[1]);

    if (N <= 0) {
        std::cerr << "N must be > 0\n";
        return 1;
    }

    std::cout << "Generating malware.exe with N = " << N << " jumps\n";

    const size_t JMP_SIZE = 5;
    const uint32_t IMAGE_BASE = 0x400000;
    const uint32_t SECTION_ALIGN = 0x1000;
    const uint32_t FILE_ALIGN = 0x200;

    // Générer le code
    std::vector<uint8_t> code;
    code.resize(N * JMP_SIZE);

    // Payload final
    std::vector<uint8_t> payload;
    for(int i=0; i<16; i++) payload.push_back(0x90); // NOPs
    payload.push_back(0x48); payload.push_back(0x31); payload.push_back(0xC0); // xor rax,rax
    payload.push_back(0xC3); // ret

    size_t payload_offset = code.size();
    code.insert(code.end(), payload.begin(), payload.end());

    // Générer les jumps
    uint32_t code_rva = 0x1000; // RVA de la section .text

    for(int j=0; j<N; j++){
        size_t file_off = j * JMP_SIZE;
        uint64_t va_jmp = IMAGE_BASE + code_rva + file_off;
        uint64_t addr_next = va_jmp + JMP_SIZE;

        uint64_t target_va;
        if(j < N-1){
            target_va = IMAGE_BASE + code_rva + ((j+1) * JMP_SIZE);
        } else {
            target_va = IMAGE_BASE + code_rva + payload_offset;
        }

        int64_t rel = (int64_t)target_va - (int64_t)addr_next;

        if(rel < INT32_MIN || rel > INT32_MAX){
            std::cerr << "ERROR: rel32 out of range\n";
            return 2;
        }

        int32_t rel32 = (int32_t)rel;

        code[file_off + 0] = 0xE9;
        code[file_off + 1] = (uint8_t)(rel32 & 0xFF);
        code[file_off + 2] = (uint8_t)((rel32 >> 8) & 0xFF);
        code[file_off + 3] = (uint8_t)((rel32 >> 16) & 0xFF);
        code[file_off + 4] = (uint8_t)((rel32 >> 24) & 0xFF);
    }

    // Aligner le code au file alignment
    size_t code_size_aligned = ((code.size() + FILE_ALIGN - 1) / FILE_ALIGN) * FILE_ALIGN;
    code.resize(code_size_aligned, 0x00);

    // Construire le PE
    std::vector<uint8_t> exe;

    // DOS Header
    DOSHeader dos = {0};
    dos.e_magic = 0x5A4D; // "MZ"
    dos.e_lfanew = 0x80;  // PE header à 0x80
    exe.insert(exe.end(), (uint8_t*)&dos, (uint8_t*)&dos + sizeof(dos));
    exe.resize(0x80, 0x00); // Padding jusqu'au PE header

    // PE Header
    PEHeader pe = {0};
    pe.signature = 0x00004550; // "PE\0\0"
    pe.machine = 0x8664;       // AMD64
    pe.numberOfSections = 1;
    pe.sizeOfOptionalHeader = sizeof(OptionalHeader64);
    pe.characteristics = 0x0022; // EXECUTABLE_IMAGE | LARGE_ADDRESS_AWARE
    exe.insert(exe.end(), (uint8_t*)&pe, (uint8_t*)&pe + sizeof(pe));

    // Optional Header
    OptionalHeader64 opt = {0};
    opt.magic = 0x20B; // PE32+
    opt.sizeOfCode = code.size();
    opt.addressOfEntryPoint = code_rva; // Entry point = début de .text
    opt.baseOfCode = code_rva;
    opt.imageBase = IMAGE_BASE;
    opt.sectionAlignment = SECTION_ALIGN;
    opt.fileAlignment = FILE_ALIGN;
    opt.majorOperatingSystemVersion = 6;
    opt.majorSubsystemVersion = 6;
    opt.sizeOfImage = code_rva + ((code.size() + SECTION_ALIGN - 1) / SECTION_ALIGN) * SECTION_ALIGN;
    opt.sizeOfHeaders = FILE_ALIGN;
    opt.subsystem = 3; // CONSOLE
    opt.numberOfRvaAndSizes = 16;
    exe.insert(exe.end(), (uint8_t*)&opt, (uint8_t*)&opt + sizeof(opt));

    // Section Header
    SectionHeader sect = {0};
    memcpy(sect.name, ".text", 5);
    sect.virtualSize = code.size();
    sect.virtualAddress = code_rva;
    sect.sizeOfRawData = code.size();
    sect.pointerToRawData = FILE_ALIGN;
    sect.characteristics = 0x60000020; // CODE | EXECUTE | READ
    exe.insert(exe.end(), (uint8_t*)&sect, (uint8_t*)&sect + sizeof(sect));

    // Padding jusqu'à FILE_ALIGN
    exe.resize(FILE_ALIGN, 0x00);

    // Code section
    exe.insert(exe.end(), code.begin(), code.end());

    // Écrire le fichier
    std::ofstream ofs("malware.exe", std::ios::binary);
    if(!ofs){
        std::cerr << "Cannot create malware.exe\n";
        return 3;
    }
    ofs.write((const char*)&exe[0], exe.size());
    ofs.close();

    std::cout << "Created malware.exe (" << exe.size() << " bytes)\n";
    std::cout << "Code size: " << code.size() << " bytes\n";
    std::cout << "Entry point RVA: 0x" << std::hex << code_rva << std::dec << "\n";

    return 0;
}