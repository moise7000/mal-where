//
// Created by ewan decima on 11/14/25.
//

#include "fuckIDA.h"

#include <vector>
#include <iostream>
#include <cstring>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

JmpChainFunc generateJumpChain(size_t N) {
    const size_t JMP_SIZE = 5;
    // Payload simple : ret
    std::vector<uint8_t> payload;
    payload.push_back(0xC3);
    size_t totalSize = N * JMP_SIZE + payload.size();

    // Allouer mémoire exécutable
    uint8_t* mem;
#ifdef _WIN32
    mem = (uint8_t*)VirtualAlloc(NULL, totalSize,
                                 MEM_COMMIT | MEM_RESERVE,
                                 PAGE_EXECUTE_READWRITE);
#else
    mem = (uint8_t*)mmap(NULL, totalSize,
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

    if(mem == NULL) {
        std::cerr << "Memory allocation failed\n";
        return NULL;
    }

    // Générer les N JMP
    for(size_t i = 0; i < N; i++){
        uint8_t* here = mem + i * JMP_SIZE;
        uint8_t* next;
        if(i < N-1)
            next = mem + (i+1) * JMP_SIZE;
        else
            next = mem + N * JMP_SIZE; // payload

        // rel32 = target - (here + 5)
        int64_t rel = (int64_t)(next - (here + JMP_SIZE));
        here[0] = 0xE9; // opcode jmp rel32
        int32_t rel32 = (int32_t) rel;
        memcpy(here + 1, &rel32, 4);
    }

    // Écrire payload
    memcpy(mem + N * JMP_SIZE, &payload[0], payload.size());

    // Retourner fonction exécutable
    return reinterpret_cast<JmpChainFunc>(mem);
}