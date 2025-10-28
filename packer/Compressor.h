//
// Created by ewan decima on 10/12/25.
//
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <vector>
#include <string>
#include <stdexcept>

namespace Compressor {
    /**
     * Fonction helper pour convertir int en string (std::to_string n'existe pas en C++98)
     */
    std::string toString(int value);

    /**
     * Compresse un tableau de bytes avec un niveau de compression spécifié
     * @param data Pointeur vers les données à compresser
     * @param size Taille des données
     * @param level Niveau de compression (0-9, ou Z_DEFAULT_COMPRESSION)
     * @return Vecteur contenant les données compressées
     */
    std::vector<unsigned char> compress(const unsigned char* data, size_t size, int level);

    /**
     * Compresse un vecteur de bytes avec un niveau de compression spécifié
     * @param data Vecteur des données à compresser
     * @param level Niveau de compression (0-9, ou Z_DEFAULT_COMPRESSION)
     * @return Vecteur contenant les données compressées
     */
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data, int level);

    /**
     * Compresse un vecteur de bytes avec le niveau de compression par défaut
     * @param data Vecteur des données à compresser
     * @return Vecteur contenant les données compressées
     */
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data);

    /**
     * Décompresse des données
     * @param data Pointeur vers les données compressées
     * @param compressedSize Taille des données compressées
     * @param originalSize Taille originale des données avant compression
     * @return Vecteur contenant les données décompressées
     */
    std::vector<unsigned char> decompress(const unsigned char* data, size_t compressedSize, size_t originalSize);

    /**
     * Décompresse un vecteur de bytes
     * @param data Vecteur des données compressées
     * @param originalSize Taille originale des données avant compression
     * @return Vecteur contenant les données décompressées
     */
    std::vector<unsigned char> decompress(const std::vector<unsigned char>& data, size_t originalSize);
}

#endif // COMPRESSOR_H