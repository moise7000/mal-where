//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include <iomanip>
#include "../Packer.h"
#include "../packer/Stub.h"

// Fonction helper pour afficher un vecteur en hexadécimal
void printHex(const std::string& label, const std::vector<unsigned char>& data) {
    std::cout << label << " (" << data.size() << " bytes): ";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

// Fonction pour comparer deux vecteurs
bool compareVectors(const std::vector<unsigned char>& v1,
                   const std::vector<unsigned char>& v2) {
    if (v1.size() != v2.size()) {
        return false;
    }
    for (size_t i = 0; i < v1.size(); ++i) {
        if (v1[i] != v2[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    std::cout << "=== TEST PACKER & STUB ===" << std::endl << std::endl;

    // Données de test
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    // Conversion en vecteur
    std::vector<unsigned char> originalData(code, code + originalSize);

    std::cout << "1. Données originales:" << std::endl;
    printHex("   Original", originalData);
    std::cout << std::endl;

    // Clé de chiffrement
    std::string key = "LHS-PC";
    std::cout << "2. Clé de chiffrement: \"" << key << "\"" << std::endl << std::endl;

    // === PACKING ===
    std::cout << "3. PACKING (compression + chiffrement):" << std::endl;
    try {
        Packer packer(key);
        std::vector<unsigned char> packed = packer.pack(originalData, originalSize);

        printHex("   Packed", packed);
        std::cout << "   Taux de compression: "
                  << originalSize << " -> " << packed.size()
                  << " bytes" << std::endl << std::endl;

        // === UNPACKING ===
        std::cout << "4. UNPACKING (déchiffrement + décompression):" << std::endl;
        Stub stub(key);
        std::vector<unsigned char> unpacked = stub.unpack(packed, originalSize);

        printHex("   Unpacked", unpacked);
        std::cout << std::endl;

        // === COMPARAISON ===
        std::cout << "5. COMPARAISON:" << std::endl;
        bool identical = compareVectors(originalData, unpacked);

        if (identical) {
            std::cout << "   ✓ SUCCESS! Les données sont identiques." << std::endl;
            std::cout << "   Le cycle pack/unpack fonctionne correctement!" << std::endl;
        } else {
            std::cout << "   ✗ ERROR! Les données sont différentes." << std::endl;
            std::cout << "   Taille originale: " << originalData.size()
                      << ", Taille après unpack: " << unpacked.size() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "ERREUR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}