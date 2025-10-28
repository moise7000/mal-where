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
* Fonction qui déchiffre et décompresse un message
*/
std::vector<unsigned char> Packer::stub(const std::vector<unsigned char>& m, size_t originalSize) {
    //TODO : déchiffrement
    std::vector<unsigned char> decrypted = cipher.decryptBytes(m)

    //TODO : décompression
    std::vector<unsigned char> decompressed = Compressor::decompress(decrypted, originalSize);

return decompressed;
}