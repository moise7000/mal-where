//
// Created by ewan decima on 10/12/25.
//


#include <iostream>
#include <iomanip>
#include <cstring>
#include <zlib.h>
#include "../packer/Compressor.h"
#include "TestingTools.h"


bool operator==(int lhs, const std::vector<unsigned char> & rhs);

int main() {
    // Données d'origine
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    try {

        std::vector<unsigned char> original(code, code + originalSize);
        std::vector<unsigned char> compressed = Compressor::compress(code, originalSize, Z_DEFAULT_COMPRESSION);
        std::vector<unsigned char> decompressed = Compressor::decompress(compressed, originalSize);


        bool identical = (decompressed.size() == originalSize) &&
                        (std::memcmp(&decompressed[0], code, originalSize) == 0);
        if (identical) TestingTools::printGreen("Test passed: compress/decompress bytes"); else TestingTools::printRed("Test failed: compress/decompress bytes");




        const int originalInt = 11122339;
        const std::vector<unsigned char> compressedInt = Compressor::compress(originalInt);
        int const decompressedInt = Compressor::decompressInt(compressedInt);
        std::cout << "Compressed int: " << decompressedInt << std::endl;
        const bool success = (originalInt == decompressedInt);
        if (success) TestingTools::printGreen("Test passed: compress/decompress int"); else TestingTools::printRed("Test failed: compress/decompress int");


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
