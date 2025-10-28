//
// Created by ewan decima on 10/28/25.
//

#ifndef MAL_WHERE_PACKER_H
#define MAL_WHERE_PACKER_H

#include "Cipher.h"
#include "Compressor.h"

class Packer {
private:
    Cipher* cipher;

public:
    /**
        * Constructeur avec la clé de chiffrement
        * @param cipherKey La clé utiliser pour le chiffrement
        */
    explicit Packer(const std::string& cipherKey);

    /**
    * Constructeur avec la clé et encodage
    * @param cipherKey La clé utilisée pour le chiffrement
    * @param encoding Type d'encodage de la clé (PLAIN, BASE64, BASE32)
    */
    Packer(const std::string& cipherKey, KeyEncoding::Type encoding);

    /**
    * Destructeur
    */
    ~Packer();

    /**
    * Fonction qui compresse et chiffre un message
    * @param m Le message chiffré et compressé sous forme de vecteur de bytes
    * @param originalSize La taille originale du message avant compression
    * @return Le message déchiffré et décompressé sous forme de vecteur de bytes
    */
    std::vector<unsigned char> pack(const std::vector<unsigned char>& m, size_t originalSize) const;

    /**
    * Fonction qui compresse et chiffre un message (retourne un string)
    * @param m Le message chiffré et compressé sous forme de vecteur de bytes
    * @param originalSize La taille originale du message avant compression
    * @return Le message déchiffré et décompressé sous forme de string
    */
    std::string packToString(const std::vector<unsigned char>& m, size_t originalSize) const;

    /**
    * Change la clé de chiffrement
    * @param newKey La nouvelle clé de déchiffrement
    */
    void setKey(const std::string& newKey);

    /**
    * Change la clé de chiffrement avec encodage
    * @param newKey La nouvelle clé de déchiffrement
    * @param encoding Type d'encodage de la clé
    */
    void setKey(const std::string& newKey, KeyEncoding::Type encoding);
};


#endif //MAL_WHERE_PACKER_H