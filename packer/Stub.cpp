//
// Created by ewan decima on 10/28/25.
//

#include "Packer.h"

/**
* Constructeur avec la clé de chiffrement
*/
Packer::Packer(const std::string& cipherKey) {
    cipher = new Cipher(cipherKey);
}

/**
* Constructeur avec la clé et encodage
*/
Packer::Packer(const std::string& cipherKey, KeyEncoding::Type encoding) {
    cipher = new Cipher(cipherKey, encoding);
}

/**
* Destructeur
*/
Packer::~Packer() {
    delete cipher;
}

/**
* Fonction qui déchiffre et décompresse un message - retourne un vecteur
*/
std::vector<unsigned char> Packer::unpack(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Conversion du vecteur en string pour decrypt
    std::string encryptedStr(m.begin(), m.end());

    // Déchiffrement
    std::string decryptedStr = cipher->decrypt(encryptedStr);

    // Conversion du string déchiffré en vecteur
    std::vector<unsigned char> decrypted(decryptedStr.begin(), decryptedStr.end());

    // Décompression
    std::vector<unsigned char> decompressed = Compressor::decompress(decrypted, originalSize);

    return decompressed;
}

/**
* Fonction qui déchiffre et décompresse un message - retourne un string
*/
std::string Packer::unpackToString(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Utilise la fonction unpack et convertit le résultat en string
    std::vector<unsigned char> result = unpack(m, originalSize);
    return std::string(result.begin(), result.end());
}

/**
* Change la clé de déchiffrement
*/
void Packer::setKey(const std::string& newKey) {
    delete cipher;
    cipher = new Cipher(newKey);
}

/**
* Change la clé de déchiffrement avec encodage
*/
void Packer::setKey(const std::string& newKey, KeyEncoding::Type encoding) {
    delete cipher;
    cipher = new Cipher(newKey, encoding);
}