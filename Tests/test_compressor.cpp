//
// Created by ewan decima on 10/12/25.
//


#include <iostream>
#include <iomanip>
#include <cstring>
#include <zlib.h>
#include "../packer/Compressor.h"
#include "TestingTools.h"



int main() {
    // Données d'origine
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    try {
        // Afficher les données originales
        std::vector<unsigned char> original(code, code + originalSize);
        TestingTools::printHex(original, "Original Data");

        // Compression
        std::vector<unsigned char> compressed = Compressor::compress(code, originalSize, Z_DEFAULT_COMPRESSION);
        std::cout << "Original size: " << originalSize << " bytes\n";
        std::cout << "Compressed size: " << compressed.size() << " bytes\n\n";

        // Afficher les données compressées
        TestingTools::printHex(compressed, "Compressed data");

        // Décompression
        std::vector<unsigned char> decompressed = Compressor::decompress(compressed, originalSize);
        std::cout << "Decompressed size: " << decompressed.size() << " bytes\n";

        // Afficher les données décompressées
        TestingTools::printHex(decompressed, "Decompressed data");

        // Vérification
        bool identical = (decompressed.size() == originalSize) &&
                        (std::memcmp(&decompressed[0], code, originalSize) == 0);
        if (identical) TestingTools::printGreen("Test passed (identical data)"); else TestingTools::printRed("Test failed");


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}