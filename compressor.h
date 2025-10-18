//
// Created by ewan decima on 10/12/25.
//
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <vector>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <zlib.h>

namespace compressor {
    // Fonction helper pour convertir int en string (std::to_string n'existe pas en C++98)
    inline std::string toString(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // Compresse un tableau de bytes
    inline std::vector<unsigned char> compress(const unsigned char* data, size_t size, int level) {
        if (size == 0) {
            return std::vector<unsigned char>();
        }

        // Calcule la taille maximale après compression
        uLongf compressedSize = compressBound(size);
        std::vector<unsigned char> compressed(compressedSize);

        // Effectue la compression
        int result = compress2(
            compressed.data(),
            &compressedSize,
            reinterpret_cast<const Bytef*>(data),
            size,
            level
        );

        if (result != Z_OK) {
            throw std::runtime_error("Erreur lors de la compression: " + toString(result));
        }

        // Redimensionne le vecteur à la taille réelle
        compressed.resize(compressedSize);
        return compressed;
    }

    // Surcharge pour std::vector avec valeur par défaut
    inline std::vector<unsigned char> compress(const std::vector<unsigned char>& data, int level) {
        return compress(&data[0], data.size(), level);
    }

    // Surcharge sans niveau de compression (utilise Z_DEFAULT_COMPRESSION)
    inline std::vector<unsigned char> compress(const std::vector<unsigned char>& data) {
        return compress(&data[0], data.size(), Z_DEFAULT_COMPRESSION);
    }

    // Décompresse des données
    inline std::vector<unsigned char> decompress(const unsigned char* data, size_t compressedSize, size_t originalSize) {
        if (compressedSize == 0) {
            return std::vector<unsigned char>();
        }

        std::vector<unsigned char> decompressed(originalSize);
        uLongf decompressedSize = originalSize;

        // Effectue la décompression
        int result = uncompress(
            decompressed.data(),
            &decompressedSize,
            data,
            compressedSize
        );

        if (result != Z_OK) {
            throw std::runtime_error("Erreur lors de la décompression: " + toString(result));
        }

        decompressed.resize(decompressedSize);
        return decompressed;
    }

    // Surcharge pour std::vector
    inline std::vector<unsigned char> decompress(const std::vector<unsigned char>& data, size_t originalSize) {
        return decompress(&data[0], data.size(), originalSize);
    }
}

#endif // COMPRESSOR_H