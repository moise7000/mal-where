//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

#include "../Stub.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Codes de couleur ANSI
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"

// Fonction utilitaire pour afficher un vecteur de bytes
void printBytes(const std::vector<unsigned char>& data) {
    std::cout << "[ ";
    for (size_t i = 0; i < data.size() && i < 20; ++i) {
        std::cout << static_cast<int>(data[i]) << " ";
    }
    if (data.size() > 20) {
        std::cout << "... (" << data.size() << " bytes total)";
    }
    std::cout << " ]" << std::endl;
}

// Test 1 : Construction et destruction basique
void testConstructorBasic() {
    std::cout << "\n=== Test 1: Constructeur basique ===" << std::endl;

    try {
        Packer packer("ma_cle_secrete");
        std::cout << COLOR_GREEN << "✓ Constructeur basique OK" << COLOR_RESET << std::endl;
    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "✗ Erreur: " << e.what() << COLOR_RESET << std::endl;
    }
}

// Test 2 : Construction avec encodage
void testConstructorWithEncoding() {
    std::cout << "\n=== Test 2: Constructeur avec encodage ===" << std::endl;

    try {
        Packer packerPlain("ma_cle", KeyEncoding::PLAIN);
        std::cout << COLOR_GREEN << "✓ Constructeur PLAIN OK" << COLOR_RESET << std::endl;

        Packer packerB64("bWFfY2xl", KeyEncoding::BASE64);
        std::cout << COLOR_GREEN << "✓ Constructeur BASE64 OK" << COLOR_RESET << std::endl;

        Packer packerB32("NV4SA3TPOI======", KeyEncoding::BASE32);
        std::cout << COLOR_GREEN << "✓ Constructeur BASE32 OK" << COLOR_RESET << std::endl;
    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "✗ Erreur: " << e.what() << COLOR_RESET << std::endl;
    }
}

// Test 3 : Déchiffrement et décompression simple
void testUnpackSimple() {
    std::cout << "\n=== Test 3: Déchiffrement et décompression (unpack) ===" << std::endl;

    try {
        Packer packer("MaSuperCleSecrete123");

        unsigned char packed_array[] = {
            0x06, 0x82, 0x87, 0x8f, 0x82, 0x90, 0x19, 0xbc,
            0x98, 0xe8, 0x83, 0x34, 0x0d, 0xba, 0x35, 0x81
        };

        std::vector<unsigned char> packed(packed_array, packed_array + sizeof(packed_array));
        size_t originalSize = sizeof(packed_array);

        std::cout << "Message chiffré/compressé: ";
        printBytes(packed);

        std::vector<unsigned char> result = packer.unpack(packed, originalSize);

        std::cout << "Message déchiffré/décompressé: ";
        printBytes(result);
        std::cout << "Taille: " << result.size() << " bytes" << std::endl;
        std::cout << COLOR_GREEN << "✓ Test unpack OK" << COLOR_RESET << std::endl;

    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "✗ Erreur: " << e.what() << COLOR_RESET << std::endl;
    }
}

// Fonction principale de test
int main() {
    std::cout << COLOR_YELLOW
              << "========================================\n"
              << "   TESTS DE LA CLASSE PACKER\n"
              << "========================================"
              << COLOR_RESET << std::endl;

    testConstructorBasic();
    testConstructorWithEncoding();
    testUnpackSimple();

    std::cout << COLOR_YELLOW
              << "\n========================================\n"
              << "   FIN DES TESTS\n"
              << "========================================"
              << COLOR_RESET << std::endl;

    return 0;
}
