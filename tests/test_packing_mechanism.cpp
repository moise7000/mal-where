//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include <iomanip>
#include "../packer/Packer.h"
#include "../packer/Stub.h"
#include "TestingTools.h"




int main() {
    TestingTools::printYellow("=========== TEST PACKER & STUB ===========");

    // Données de test
    unsigned char code[] = {0x56, 0x34, 0x12, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t originalSize = sizeof(code);

    // Conversion en vecteur
    std::vector<unsigned char> originalData(code, code + originalSize);

    std::cout << "1. Original data:" << std::endl;
    TestingTools::printHex(originalData, "Original");
    std::cout << std::endl;

    // Clé de chiffrement
    std::string key = "LHS-PC";
    std::cout << "2. Encryption key: \"" << key << "\"" << std::endl << std::endl;

    // === PACKING ===
    std::cout << "3. PACKING (compression + encryption):" << std::endl;
    try {
        Packer packer(key);
        std::vector<unsigned char> packed = packer.pack(originalData, originalSize);

        TestingTools::printHex(packed, "Packed");
        std::cout << "   Compression rate: "
                  << originalSize << " -> " << packed.size()
                  << " bytes" << std::endl << std::endl;

        // === UNPACKING ===
        std::cout << "4. UNPACKING (decryption + decompression):" << std::endl;
        Stub stub(key);
        std::vector<unsigned char> unpacked = stub.unpack(packed, originalSize);

        TestingTools::printHex(unpacked, "Unpacked");
        std::cout << std::endl;

        // === COMPARAISON ===
        std::cout << "5. COMPARAISON:" << std::endl;
        bool identical = TestingTools::compareVectors(originalData, unpacked);

        if (identical) {
            TestingTools::printGreen("Test passed (identical data)");

        } else {
            TestingTools::printRed("Test failed");
            std::cout << "   Original size: " << originalData.size()
                      << ", Size after unpack: " << unpacked.size() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}