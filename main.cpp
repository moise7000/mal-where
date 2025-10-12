//
// Created by ewan decima on 10/12/25.
//


#include <iostream>
#include "compressor.h"

int main() {
    // Données d'origine
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    try {
        // Compression
        auto compressed = compressor::compress(code, originalSize);
        std::cout << "Taille originale: " << originalSize << " bytes\n";
        std::cout << "Taille compressée: " << compressed.size() << " bytes\n";


        // Décompression
        auto decompressed = compressor::decompress(compressed, originalSize);
        std::cout << "Taille décompressée: " << decompressed.size() << " bytes\n";

        // Vérification
        bool identical = (decompressed.size() == originalSize) &&
                        (memcmp(decompressed.data(), code, originalSize) == 0);
        std::cout << "Données identiques: " << (identical ? "Oui" : "Non") << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << "\n";
        return 1;
    }

    return 0;
}