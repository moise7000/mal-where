//
// Created by ewan decima on 10/12/25.
//


#include <iostream>
#include <iomanip>
#include <cstring>
#include "Compressor.h"

// Fonction pour afficher les données en hexadécimal
void printHex(const std::vector<unsigned char>& data, const std::string& label) {
    std::cout << label << " (" << data.size() << " bytes):\n";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";  // Nouvelle ligne tous les 16 bytes
    }
    std::cout << std::dec << "\n\n";  // Retour en décimal
}

int main() {
    // Données d'origine
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    try {
        // Afficher les données originales
        std::vector<unsigned char> original(code, code + originalSize);
        printHex(original, "Données originales");

        // Compression
        std::vector<unsigned char> compressed = compressor::compress(code, originalSize, Z_DEFAULT_COMPRESSION);
        std::cout << "Taille originale: " << originalSize << " bytes\n";
        std::cout << "Taille compressée: " << compressed.size() << " bytes\n\n";

        // Afficher les données compressées
        printHex(compressed, "Données compressées");

        // Décompression
        std::vector<unsigned char> decompressed = compressor::decompress(compressed, originalSize);
        std::cout << "Taille décompressée: " << decompressed.size() << " bytes\n";

        // Afficher les données décompressées
        printHex(decompressed, "Données décompressées");

        // Vérification
        bool identical = (decompressed.size() == originalSize) &&
                        (std::memcmp(&decompressed[0], code, originalSize) == 0);
        std::cout << "Données identiques: " << (identical ? "Oui" : "Non") << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << "\n";
        return 1;
    }

    return 0;
}