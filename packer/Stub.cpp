//
// Created by ewan decima on 10/28/25.
//

#include "Stub.h"

/**
* Constructeur avec la clÃ© de chiffrement
*/
Stub::Stub(const std::string& cipherKey) {
    cipher = new Cipher(cipherKey);
}

/**
* Constructeur avec la clÃ© et encodage
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
* Fonction qui dÃ©chiffre et dÃ©compresse un message - retourne un vecteur
*/
std::vector<unsigned char> Stub::unpack(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Déchiffrement (utilise decryptBytes au lieu de decrypt)
    std::vector<unsigned char> decrypted = cipher->decryptBytes(m);

    // Décompression
    std::vector<unsigned char> decompressed = Compressor::decompress(decrypted, originalSize);

    return decompressed;
}



/**
 * Fonction qui déchiffre et décompresse un entier
 */
int Stub::unpack(const std::vector<unsigned char>& packed) const {
    // Déchiffrement des données
    std::vector<unsigned char> decrypted = cipher->decryptBytes(packed);

    // Décompression de l'entier
    int result = Compressor::decompressInt(decrypted);

    return result;
}


/**
* Fonction qui dÃ©chiffre et dÃ©compresse un message - retourne un string
*/
std::string Stub::unpackToString(const std::vector<unsigned char>& m, size_t originalSize) const {
    // Utilise la fonction unpack et convertit le rÃ©sultat en string
    std::vector<unsigned char> result = unpack(m, originalSize);
    return std::string(result.begin(), result.end());
}

/**
* Change la clé de chiffrement
*/
void Stub::setKey(const std::string& newKey) {
    delete cipher;
    cipher = new Cipher(newKey);
}

/**
* Change la clÃ© de dÃ©chiffrement avec encodage
*/
void Stub::setKey(const std::string& newKey, KeyEncoding::Type encoding) {
    delete cipher;
    cipher = new Cipher(newKey, encoding);
}