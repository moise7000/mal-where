//
// Created by ewan decima on 10/28/25.
//

#include "Packer.h"


/**
* Constructeur avec la clé de chiffrement
*/
Packer::Packer(const std::string& ciperKey) {
    cipher = new Cipher(ciperKey);
}


/**
* Constructeur avec la clé et encodage
*/
Packer::Packer(const std::string& ciperKey, KeyEncoding::Type encoding) {
    cipher = new Cipher(ciperKey, encoding);
}


/**
* Destructeur
*/
Packer::~Packer() { delete cipher; }


/**
* Fonction qui compresse et chiffre un message - retourne un vecteur
*/
std::vector<unsigned char> Packer::pack(const std::vector<unsigned char>& m, size_t originalSize) const {
    //TODO : Compression des données
    std::vector<unsigned char> compressed = Compressor::comress(m)

    //TODO : Chiffrement des données compressées
    std::vector<unsigned char> encrypted = cipher->encryptBytes(compressed)
}

/**
* Fonction qui compresse et chiffre un message (retourne un string)
*/
std::string Packer::packToString(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Utilise la fonction pack pour obtenir les données chiffrées
    std::vector<unsigned char> packed = pack(m, originalSize);

    // Convertit le résultat en string
    return std::string(packed.begin(), packed.end());
}


/**
* Change la clé de chiffrement
*/
void Packer::setKey(const std::string& newKey) {
    cipher->setKey(newKey);
}


/**
* Change la clé de chiffrement avec encodage
*/
void Packer::setKey(const std::string& newKey, KeyEncoding::Type encoding) {
    cipher->setKey(newKey, encoding);
}