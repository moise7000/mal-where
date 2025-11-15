//
// Created by ewan decima on 11/15/25.
//



#ifndef MAL_WHERE_HASH_FUNCTION_H
#define MAL_WHERE_HASH_FUNCTION_H

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @brief Structure pour stocker le digest (hash résultat).
 */
struct Digest {
    unsigned int h[8];

    Digest() {
        for (int i = 0; i < 8; i++) {
            h[i] = 0;
        }
    }

    /**
     * @brief Convertit le digest en chaîne hexadécimale.
     */
    std::string to_hex() const {
        std::stringstream ss;
        for (int i = 0; i < 8; i++) {
            ss << std::hex << std::setw(8) << std::setfill('0') << h[i];
        }
        return ss.str();
    }
};

// =================================================================
// CONSTANTES ET FONCTIONS AUXILIAIRES SHA-256
// =================================================================

// Constantes SHA-256 (premiers 32 bits des racines cubiques des 64 premiers nombres premiers)
static const unsigned int K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Valeurs initiales de hash (premiers 32 bits des racines carrées des 8 premiers nombres premiers)
static const unsigned int H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/**
 * @brief Rotation droite de n bits.
 */
inline unsigned int rotr(unsigned int x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

/**
 * @brief Fonction Ch (Choose).
 */
inline unsigned int ch(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (~x & z);
}

/**
 * @brief Fonction Maj (Majority).
 */
inline unsigned int maj(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

/**
 * @brief Fonction Sigma0.
 */
inline unsigned int sigma0(unsigned int x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

/**
 * @brief Fonction Sigma1.
 */
inline unsigned int sigma1(unsigned int x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

/**
 * @brief Fonction sigma0 (minuscule).
 */
inline unsigned int sigma0_min(unsigned int x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

/**
 * @brief Fonction sigma1 (minuscule).
 */
inline unsigned int sigma1_min(unsigned int x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

// =================================================================
// FONCTION DE HACHAGE SHA-256
// =================================================================

/**
 * @brief Calcule le hash SHA-256 d'une chaîne.
 *
 * Cette implémentation suit la spécification FIPS 180-4.
 *
 * @param message La chaîne à hasher (la clé K).
 * @return Le digest X = H(K).
 */
Digest sha256(const std::string& message);

// =================================================================
// FONCTIONS UTILITAIRES
// =================================================================

/**
 * @brief Fonction de hachage qui retourne une chaîne hexadécimale.
 *
 * @param input La chaîne d'entrée à hasher.
 * @return Le digest en format hexadécimal.
 */
std::string hash_function(const std::string& input);

/**
 * @brief Vérifie si un digest correspond à une entrée.
 *
 * @param input L'entrée à vérifier.
 * @param digest Le digest à vérifier (format hexadécimal).
 * @return true si hash(input) == digest, false sinon.
 */
bool verify_hash(const std::string& input, const std::string& digest);

#endif //MAL_WHERE_HASH_FUNCTION_H