//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

#include "../packer/Stub.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <windows.h> // Pour changer la couleur du texte



// Test 1 : Construction et destruction basique
void testConstructorBasic() {
    std::cout << "\n=== Test 1: Default ===" << std::endl;

    try {
        Packer packer("ma_cle_secrete");
        TestingTools::printGreen(" Default OK");
    } catch (const std::exception& e) {
        TestingTools::printRed(std::string(" Error: ") + e.what());
    }
}

// Test 2 : Construction avec encodage
void testConstructorWithEncoding() {
    std::cout << "\n=== Test 2: Encoding ===" << std::endl;

    try {
        Stub StubPlain("ma_cle", KeyEncoding::PLAIN);
        TestingTools::printGreen("PLAIN OK");

        Stub StubB64("bWFfY2xl", KeyEncoding::BASE64);
        TestingTools::printGreen("BASE64 OK");

        Stub StubB32("NV4SA3TPOI======", KeyEncoding::BASE32);
        TestingTools::printGreen("BASE32 OK");
    } catch (const std::exception& e) {
        TestingTools::printRed(std::string(" Error: ") + e.what());
    }
}

// Test 3 : Déchiffrement et décompression simple
void testUnpackSimple() {
    std::cout << "\n=== Test 3: Decryption and decompression (unpack) ===" << std::endl;

    try {
        Stub Stub("MaSuperCleSecrete123");

        unsigned char packed_array[] = {
            0x06, 0x82, 0x87, 0x8f, 0x82, 0x90, 0x19, 0xbc,
            0x98, 0xe8, 0x83, 0x34, 0x0d, 0xba, 0x35, 0x81
        };

        std::vector<unsigned char> packed(packed_array, packed_array + sizeof(packed_array));
        size_t originalSize = sizeof(packed_array);

        std::cout << "Encrypted/compressed message: ";
        TestingTools::printBytes(packed);

        std::vector<unsigned char> result = Stub.unpack(packed, originalSize);

        std::cout << "Message decrypted/decompressed: ";
        TestingTools::printBytes(result);
        std::cout << "Taille: " << result.size() << " bytes" << std::endl;
        TestingTools::printGreen(" Test unpack OK");

    } catch (const std::exception& e) {
        printRed(std::string(" Error: ") + e.what());
    }
}

// Fonction principale de test
int main() {
    TestingTools::printYellow("========================================");
    TestingTools::printYellow("   Stub Class Tests");
    TestingTools::printYellow("========================================");

    testConstructorBasic();
    testConstructorWithEncoding();
    testUnpackSimple();

    TestingTools::printYellow("\n========================================");
    TestingTools::printYellow("   END");
    TestingTools::printYellow("========================================\n");

    return 0;
}
