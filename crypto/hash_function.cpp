#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "../obfuscation_methods/hash_function.h"


/**
 * @brief Calcule le hash SHA-256 d'une chaîne.
 * 
 * Cette implémentation suit la spécification FIPS 180-4.
 * 
 * @param message La chaîne à hasher (la clé K).
 * @return Le digest X = H(K).
 */
Digest sha256(const std::string& message) {
    // Initialisation des valeurs de hash
    unsigned int h[8];
    for (int i = 0; i < 8; i++) {
        h[i] = H0[i];
    }
    
    // Préparation du message (padding)
    std::string padded = message;
    size_t original_length = message.length();
    size_t original_bits = original_length * 8;
    
    // Ajouter le bit '1' (représenté par 0x80)
    padded += (char)0x80;
    
    // Ajouter des zéros jusqu'à ce que la longueur soit ≡ 448 mod 512 (en bits)
    // Soit ≡ 56 mod 64 (en octets)
    while ((padded.length() % 64) != 56) {
        padded += (char)0x00;
    }
    
    // Ajouter la longueur originale en bits (64 bits, big-endian)
    for (int i = 7; i >= 0; i--) {
        padded += (char)((original_bits >> (i * 8)) & 0xFF);
    }
    
    // Traitement par blocs de 512 bits (64 octets)
    for (size_t chunk = 0; chunk < padded.length(); chunk += 64) {
        unsigned int w[64];
        
        // Préparer le message schedule (w[0..15])
        for (int i = 0; i < 16; i++) {
            w[i] = ((unsigned int)(unsigned char)padded[chunk + i * 4] << 24) |
                   ((unsigned int)(unsigned char)padded[chunk + i * 4 + 1] << 16) |
                   ((unsigned int)(unsigned char)padded[chunk + i * 4 + 2] << 8) |
                   ((unsigned int)(unsigned char)padded[chunk + i * 4 + 3]);
        }
        
        // Étendre le message schedule (w[16..63])
        for (int i = 16; i < 64; i++) {
            w[i] = sigma1_min(w[i - 2]) + w[i - 7] + sigma0_min(w[i - 15]) + w[i - 16];
        }
        
        // Initialiser les variables de travail
        unsigned int a = h[0];
        unsigned int b = h[1];
        unsigned int c = h[2];
        unsigned int d = h[3];
        unsigned int e = h[4];
        unsigned int f = h[5];
        unsigned int g = h[6];
        unsigned int hh = h[7];
        
        // Boucle principale
        for (int i = 0; i < 64; i++) {
            unsigned int t1 = hh + sigma1(e) + ch(e, f, g) + K[i] + w[i];
            unsigned int t2 = sigma0(a) + maj(a, b, c);
            hh = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }
        
        // Ajouter ce bloc aux résultats intermédiaires
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += hh;
    }
    
    // Créer le digest final
    Digest result;
    for (int i = 0; i < 8; i++) {
        result.h[i] = h[i];
    }
    
    return result;
}

// =================================================================
// FONCTIONS UTILITAIRES
// =================================================================

/**
 * @brief Fonction de hachage qui retourne une chaîne hexadécimale.
 * 
 * @param input La chaîne d'entrée à hasher.
 * @return Le digest en format hexadécimal.
 */
std::string hash_function(const std::string& input) {
    return sha256(input).to_hex();
}

/**
 * @brief Vérifie si un digest correspond à une entrée.
 * 
 * @param input L'entrée à vérifier.
 * @param digest Le digest à vérifier (format hexadécimal).
 * @return true si hash(input) == digest, false sinon.
 */
bool verify_hash(const std::string& input, const std::string& digest) {
    return hash_function(input) == digest;
}
