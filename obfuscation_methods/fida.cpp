// fida.cpp - Compatible GCC 4.4.7
#include <stdint.h>      // Au lieu de <cstdint>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <climits>

int main(int argc, char** argv){
    int N = 2000;
    uint64_t base = 0x400000;

    if(argc >= 2) N = std::atoi(argv[1]);
    if(argc >= 3) base = strtoull(argv[2], NULL, 0);  // strtoull sans std::

    if (N <= 0) {
        std::cerr << "N must be > 0\n";
        return 1;
    }

    std::cout << "Generating chain.bin with N = " << N << ", base = 0x"
              << std::hex << base << std::dec << "\n";

    std::vector<uint8_t> blob;
    const size_t JMP_SIZE = 5;

    blob.resize(N * JMP_SIZE);

    std::vector<uint8_t> payload;
    const int NOP_COUNT = 16;
    for(int i=0; i<NOP_COUNT; i++) payload.push_back(0x90);
    payload.push_back(0x48);
    payload.push_back(0x31);
    payload.push_back(0xC0);
    payload.push_back(0xC3);

    size_t payload_offset_in_file = blob.size();
    size_t align = 16;
    size_t pad = (align - (payload_offset_in_file % align)) % align;
    for(size_t i=0; i<pad; i++) blob.push_back(0x90);
    payload_offset_in_file = blob.size();
    blob.insert(blob.end(), payload.begin(), payload.end());

    for(int j=0; j<N; j++){
        uint64_t file_off = (uint64_t)j * JMP_SIZE;
        uint64_t va_jmp = base + file_off;
        uint64_t addr_next = va_jmp + JMP_SIZE;

        uint64_t target_va;
        if(j < N-1){
            target_va = base + ((uint64_t)(j+1) * JMP_SIZE);
        } else {
            target_va = base + payload_offset_in_file;
        }

        int64_t rel = (int64_t)target_va - (int64_t)addr_next;

        if(rel < INT32_MIN || rel > INT32_MAX){
            std::cerr << "ERROR: rel32 out of range at index " << j << "\n";
            return 2;
        }

        int32_t rel32 = (int32_t)rel;
        size_t write_at = (size_t)file_off;

        blob[write_at + 0] = 0xE9;
        blob[write_at + 1] = (uint8_t)(rel32 & 0xFF);
        blob[write_at + 2] = (uint8_t)((rel32 >> 8) & 0xFF);
        blob[write_at + 3] = (uint8_t)((rel32 >> 16) & 0xFF);
        blob[write_at + 4] = (uint8_t)((rel32 >> 24) & 0xFF);
    }

    std::ofstream ofs("chain.bin", std::ios::binary);
    if(!ofs){
        std::cerr << "Cannot open chain.bin for writing\n";
        return 3;
    }
    ofs.write((const char*)&blob[0], blob.size());
    ofs.close();

    std::cout << "Wrote chain.bin (" << blob.size() << " bytes).\n";
    std::cout << "Payload file offset = " << payload_offset_in_file << "\n";

    return 0;
}