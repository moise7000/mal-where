//
// Created by ewan decima on 10/18/25.
//

#include "../packer/Cipher.h"
#include "TestingTools.h"
#include <iostream>
#include <iomanip>
/**
void printBytes(const std::string& label, const unsigned char* data, size_t size) {
    std::cout << label << " ";
    for (size_t i = 0; i < size; i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]);
        if (i < size - 1) std::cout << ", ";
    }
    std::cout << std::dec << std::endl;
}

void printVector(const std::string& label, const std::vector<unsigned char>& data) {
    std::cout << label << " ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]);
        if (i < data.size() - 1) std::cout << ", ";
    }
    std::cout << std::dec << std::endl;
}
*/

int main() {
    try {
        // Tableau de bytes original
        unsigned char code[] = {0x78, 0x9c, 0x0b, 0x33, 0x11, 0xaa, 0x98, 0xb5, 0xe7, 0xde, 0x07, 0x00, 0x0e, 0x30, 0x04, 0x39};
        size_t originalSize = sizeof(code);

        std::cout << "=== Chiffrement de tableau de bytes ===" << std::endl << std::endl;

        // Affichage des données originales
        TestingTools::printBytes("Données originales:", code, originalSize);
        std::cout << "Taille: " << originalSize << " bytes" << std::endl << std::endl;

        // Création du cipher avec une clé
        std::string key = "MaSuperCleSecrete123";
        Cipher cipher(key);

        std::cout << "Clé utilisée: " << key << std::endl;
        std::cout << "Longueur de la clé: " << cipher.getKeyLength() << " bytes" << std::endl;
        std::cout << "Clé en Base64: " << cipher.getKeyAsBase64() << std::endl << std::endl;

        // Conversion du tableau en vector pour le chiffrement
        std::vector<unsigned char> dataVec(code, code + originalSize);

        // Chiffrement
        std::vector<unsigned char> encrypted = cipher.encryptBytes(dataVec);
        TestingTools::printVector("Données chiffrées:", encrypted);

        // Affichage en hexadécimal
        std::string hexEncrypted = Cipher::bytesToHex(encrypted);
        std::cout << "Hex: " << hexEncrypted << std::endl << std::endl;

        // Déchiffrement
        std::vector<unsigned char> decrypted = cipher.decryptBytes(encrypted);
        TestingTools::printVector("Données déchiffrées:", decrypted);
        std::cout << std::endl;

        // Vérification
        bool success = (dataVec == decrypted);
        std::cout << "Vérification: " << (success ? "SUCCÈS ✓" : "ÉCHEC ✗") << std::endl;
        std::cout << std::endl;

        // Exemple avec stockage du résultat dans un tableau C
        std::cout << "=== Code C pour tableau chiffré ===" << std::endl;
        std::cout << "unsigned char encryptedCode[] = {";
        for (size_t i = 0; i < encrypted.size(); i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(encrypted[i]);
            if (i < encrypted.size() - 1) std::cout << ", ";
        }
        std::cout << std::dec << "};" << std::endl;
        std::cout << "size_t encryptedSize = " << encrypted.size() << ";" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}