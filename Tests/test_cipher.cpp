//
// Created by ewan decima on 10/18/25.
//

#include "../packer/Cipher.h"
#include "TestingTools.h"
#include <iostream>
#include <iomanip>


int main() {
    try {
        // Tableau de bytes original
        unsigned char code[] = {0x78, 0x9c, 0x0b, 0x33, 0x11, 0xaa, 0x98, 0xb5, 0xe7, 0xde, 0x07, 0x00, 0x0e, 0x30, 0x04, 0x39};
        size_t originalSize = sizeof(code);

        TestingTools::printYellow("=========== Byte array encryption ===========");

        // Affichage des données originales
        TestingTools::printBytes("Original data:", code, originalSize);
        std::cout << "Size: " << originalSize << " bytes" << std::endl << std::endl;

        // Création du cipher avec une clé
        std::string key = "MaSuperCleSecrete123";
        Cipher cipher(key);

        std::cout << "Used Key: " << key << std::endl;
        std::cout << "Key lenght: " << cipher.getKeyLength() << " bytes" << std::endl;
        std::cout << "Base64 key: " << cipher.getKeyAsBase64() << std::endl << std::endl;

        // Conversion du tableau en vector pour le chiffrement
        std::vector<unsigned char> dataVec(code, code + originalSize);

        // Chiffrement
        std::vector<unsigned char> encrypted = cipher.encryptBytes(dataVec);
        TestingTools::printVector("Encrypted data:", encrypted);

        // Affichage en hexadécimal
        std::string hexEncrypted = Cipher::bytesToHex(encrypted);
        std::cout << "Hex: " << hexEncrypted << std::endl << std::endl;

        // Déchiffrement
        std::vector<unsigned char> decrypted = cipher.decryptBytes(encrypted);
        TestingTools::printVector("Decrypted data:", decrypted);
        std::cout << std::endl;

        // Vérification
        bool success = (dataVec == decrypted);
        if (success) {TestingTools::printGreen("Test passed")} else {TestingTools::printRed("Test failed")}



        // Exemple avec stockage du résultat dans un tableau C
        std::cout << "=========== C code for encrypted array ===========" << std::endl;
        std::cout << "unsigned char encryptedCode[] = {";
        for (size_t i = 0; i < encrypted.size(); i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(encrypted[i]);
            if (i < encrypted.size() - 1) std::cout << ", ";
        }
        std::cout << std::dec << "};" << std::endl;
        std::cout << "size_t encryptedSize = " << encrypted.size() << ";" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}