//
// Created by ewan decima on 10/18/25.
//

#include "Cipher.h"
#include <iostream>

int main() {

    Cipher cipher("Z2l0aHViLmNvbS9tb2lzZTcwMDAvTWFsd2FyZS1UZWxlY29tCg==", KeyEncoding::BASE64);

    // Chiffrer un message
    std::string message = "Bonjour!";
    std::string encrypted = cipher.encrypt(message);
    std::string decrypted = cipher.decrypt(encrypted);

    std::cout << "Original: " << message << std::endl;
    std::cout << "Déchiffré: " << decrypted << std::endl;

    // Chiffrer des bytes
    std::vector<unsigned char> data;
    data.push_back(0x48); // H
    data.push_back(0x65); // e
    data.push_back(0x6C); // l
    data.push_back(0x6C); // l
    data.push_back(0x6F); // o

    std::vector<unsigned char> encBytes = cipher.encryptBytes(data);

    // Utiliser les méthodes statiques
    std::cout << "Hex: " << Cipher::bytesToHex(encBytes) << std::endl;

    return 0;
}