//
// Created by ewan decima on 10/28/25.
//

#include "Compressor.h"
#include <sstream>
#include <zlib.h>

namespace Compressor {

    // Fonction helper pour convertir int en string
    std::string toString(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // Compresse un tableau de bytes
    std::vector<unsigned char> compress(const unsigned char* data, size_t size, int level) {
        if (size == 0) {
            return std::vector<unsigned char>();
        }

        // Calcule la taille maximale après compression
        uLongf compressedSize = compressBound(static_cast<uLong>(size));
        std::vector<unsigned char> compressed(compressedSize);

        // Effectue la compression
        int result = compress2(
            &compressed[0],
            &compressedSize,
            reinterpret_cast<const Bytef*>(data),
            static_cast<uLong>(size),
            level
        );

        if (result != Z_OK) {
            throw std::runtime_error("Erreur lors de la compression: " + toString(result));
        }

        // Redimensionne le vecteur à la taille réelle
        compressed.resize(static_cast<size_t>(compressedSize));
        return compressed;
    }

    // Surcharge pour std::vector avec niveau de compression
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data, int level) {
        if (data.empty()) {
            return std::vector<unsigned char>();
        }
        return compress(&data[0], data.size(), level);
    }

    // Surcharge sans niveau de compression (utilise Z_DEFAULT_COMPRESSION)
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data) {
        if (data.empty()) {
            return std::vector<unsigned char>();
        }
        return compress(&data[0], data.size(), Z_DEFAULT_COMPRESSION);
    }

    // Décompresse des données
    std::vector<unsigned char> decompress(const unsigned char* data, size_t compressedSize, size_t originalSize) {
        if (compressedSize == 0) {
            return std::vector<unsigned char>();
        }

        std::vector<unsigned char> decompressed(originalSize);
        uLongf decompressedSize = static_cast<uLongf>(originalSize);

        // Effectue la décompression
        int result = uncompress(
            &decompressed[0],
            &decompressedSize,
            reinterpret_cast<const Bytef*>(data),
            static_cast<uLong>(compressedSize)
        );

        if (result != Z_OK) {
            throw std::runtime_error("Erreur lors de la décompression: " + toString(result));
        }

        decompressed.resize(static_cast<size_t>(decompressedSize));
        return decompressed;
    }

    // Surcharge pour std::vector
    std::vector<unsigned char> decompress(const std::vector<unsigned char>& data, size_t originalSize) {
        if (data.empty()) {
            return std::vector<unsigned char>();
        }
        return decompress(&data[0], data.size(), originalSize);
    }

    // Compresse un entier avec niveau de compression
    std::vector<unsigned char> compress(int value, int level) {
        // Convertir l'entier en bytes (4 bytes pour un int)
        std::vector<unsigned char> data(4);
        data[0] = (value >> 24) & 0xFF;
        data[1] = (value >> 16) & 0xFF;
        data[2] = (value >> 8) & 0xFF;
        data[3] = value & 0xFF;

        // Compresser les bytes
        return compress(data, level);
    }

    // Compresse un entier avec niveau de compression par défaut
    std::vector<unsigned char> compress(int value) {
        return compress(value, Z_DEFAULT_COMPRESSION);
    }

    // Décompresse un entier
    int decompressInt(const std::vector<unsigned char>& data) {
        // Décompresser les données (taille originale = 4 bytes)
        std::vector<unsigned char> decompressed = decompress(data, 4);

        if (decompressed.size() != 4) {
            throw std::runtime_error("Erreur: taille incorrecte après décompression de l'entier");
        }

        // Reconvertir en int
        int result = 0;
        result |= (static_cast<int>(decompressed[0]) << 24);
        result |= (static_cast<int>(decompressed[1]) << 16);
        result |= (static_cast<int>(decompressed[2]) << 8);
        result |= static_cast<int>(decompressed[3]);

        return result;
    }
} // namespace Compressor