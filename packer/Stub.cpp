//
// Created by ewan decima on 10/28/25.
//

#include "Stub.h"

/**
* Fonction qui déchiffre et décompresse un message - retourne un vecteur
*/
std::vector<unsigned char> Stub::unpack(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Étape 1: Déchiffrement des données (inverse de encryptBytes)
    std::vector<unsigned char> decrypted = cipher->decryptBytes(m);

    // Étape 2: Décompression des données
    std::vector<unsigned char> decompressed = Compressor::decompress(decrypted, originalSize);

    return decompressed;
}

/**
* Fonction qui déchiffre et décompresse un message - retourne un string
*/
std::string Stub::unpackToString(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Utilise la fonction unpack et convertit le résultat en string
    std::vector<unsigned char> result = unpack(m, originalSize);
    return std::string(result.begin(), result.end());
}

/**
* Change la clé de déchiffrement
*/
void Stub::setKey(const std::string& newKey) {
    cipher->setKey(newKey);
}

/**
* Change la clé de déchiffrement avec encodage
*/
void Stub::setKey(const std::string& newKey, KeyEncoding::Type encoding) {
    cipher->setKey(newKey, encoding);
} Constructeur avec la clé de chiffrement
*/
Stub::Stub(const std::string& cipherKey) {
    cipher = new Cipher(cipherKey);
}

/**
* Constructeur avec la clé et encodage
*/
Stub::Stub(const std::string& cipherKey, KeyEncoding::Type encoding) {
    cipher = new Cipher(cipherKey, encoding);
}

/**
* Destructeur
*/
Stub::~Stub() {
    delete cipher;
}

/**
*