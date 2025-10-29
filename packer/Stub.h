//
// Created by ewan decima on 10/28/25.
//

#ifndef MAL_WHERE_STUB_H
#define MAL_WHERE_STUB_H
#include "Compressor.h"
#include "Cipher.h"

class Stub {
private:
    Cipher* cipher;

public:
    /**
    * Constructeur avec la clé de chiffrement
    * @param cipherKey La clé utiliser pour le chiffrement
    */
    explicit Stub(const std::string& cipherKey);

    /**
    * Constructeur avec la clé et encodage
    * @param cipherKey La clé utilisée pour le chiffrement
    * @param encoding Type d'encodage de la clé (PLAIN, BASE64, BASE32)
    */
    Stub(const std::string& cipherKey, KeyEncoding::Type encoding);

    /**
    * Destructeur
    */
    ~Stub();

    /**
    * Fonction qui déchiffre et décompresse un message
    * @param m Le message chiffré et compressé sous forme de vecteur de bytes
    * @param originalSize La taille originale du message avant compression
    * @return Le message déchiffré et décompressé sous forme de vecteur de bytes
    */
    std::vector<unsigned char> unpack(const std::vector<unsigned char>& m, size_t originalSize) const;

    /**
    * Fonction qui déchiffre et décompresse un message (retourne un string)
    * @param m Le message chiffré et compressé sous forme de vecteur de bytes
    * @param originalSize La taille originale du message avant compression
    * @return Le message déchiffré et décompressé sous forme de string
    */
    std::string unpackToString(const std::vector<unsigned char>& m, size_t originalSize) const;

    /**
    * Change la clé de déchiffrement
    * @param newKey La nouvelle clé de déchiffrement
    */
    void setKey(const std::string& newKey);

    /**
    * Change la clé de déchiffrement avec encodage
    * @param newKey La nouvelle clé de déchiffrement
    * @param encoding Type d'encodage de la clé
    */
    void setKey(const std::string& newKey, KeyEncoding::Type encoding);
};

#endif