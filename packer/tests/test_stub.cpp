//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

#include "../Stub.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

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
        std::cout << "✓ Constructeur basique OK" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 2 : Construction avec encodage
void testConstructorWithEncoding() {
    std::cout << "\n=== Test 2: Constructeur avec encodage ===" << std::endl;

    try {
        Packer packerPlain("ma_cle", KeyEncoding::PLAIN);
        std::cout << "✓ Constructeur PLAIN OK" << std::endl;

        Packer packerB64("bWFfY2xl", KeyEncoding::BASE64);
        std::cout << "✓ Constructeur BASE64 OK" << std::endl;

        Packer packerB32("NV4SA3TPOI======", KeyEncoding::BASE32);
        std::cout << "✓ Constructeur BASE32 OK" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 3 : Déchiffrement et décompression simple
void testUnpackSimple() {
    std::cout << "\n=== Test 3: Déchiffrement et décompression (unpack) ===" << std::endl;

    try {
        Packer packer("MaSuperCleSecrete123");

        unsigned char plain[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};



        unsigned char packed_array[] = {0x06, 0x82, 0x87, 0x8f, 0x82, 0x90, 0x19, 0xbc, 0x98, 0xe8, 0x83, 0x34, 0x0d, 0xba, 0x35, 0x81};

        // Conversion du tableau en vecteur
        std::vector<unsigned char> packed(packed_array, packed_array + sizeof(packed_array));
        size_t originalSize = sizeof(packed_array);

        std::cout << "Message chiffré/compressé: ";
        printBytes(packed);  // Maintenant packed est un vector

        std::vector<unsigned char> result = packer.unpack(packed, originalSize);

        std::cout << "Message déchiffré/décompressé: ";
        printBytes(result);
        std::cout << "Taille: " << result.size() << " bytes" << std::endl;
        std::cout << "✓ Test unpack OK" << std::endl;
        g

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}










// Fonction principale de test
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   TESTS DE LA CLASSE PACKER" << std::endl;
    std::cout << "========================================" << std::endl;

    testConstructorBasic();
    testConstructorWithEncoding();
    testUnpackSimple();


    std::cout << "\n========================================" << std::endl;
    std::cout << "   FIN DES TESTS" << std::endl;
    std::cout << "========================================\n" << std::endl;

    return 0;
}