#ifndef CIPHER_H
#define CIPHER_H

#include <string>
#include <vector>
#include <stdexcept>

/**
 * Namespace pour les différents encodages de clé
 */
namespace KeyEncoding {
    enum Type {
        PLAIN,    // Clé en clair
        BASE64,   // Clé encodée en Base64
        BASE32    // Clé encodée en Base32
    };
}

/**
 * Classe pour le chiffrement/déchiffrement XOR avec clé variable
 */
class Cipher {
private:
    std::string key;

    // Tables d'encodage (déclarées ici, définies dans le .cpp)
    static const std::string BASE64_CHARS;
    static const std::string BASE32_CHARS;

    // Méthodes privées de décodage
    static std::string decodeBase64(const std::string& encoded);
    static std::string decodeBase32(const std::string& encoded);

    // Génère une clé étendue
    std::vector<unsigned char> generateExtendedKey(size_t length) const;

public:
    /**
     * Constructeur avec clé personnalisée (plain)
     */
    explicit Cipher(const std::string& customKey);

    /**
     * Constructeur avec clé personnalisée et encodage
     */
    Cipher(const std::string& customKey, KeyEncoding::Type encoding);

    /**
     * Destructeur
     */
    ~Cipher();

    /**
     * Chiffre un message texte
     */
    std::string encrypt(const std::string& message) const;

    /**
     * Déchiffre un message texte
     */
    std::string decrypt(const std::string& encrypted) const;

    /**
     * Chiffre un entier
    */
     int encrypt(const int i) const;

    /**
     * Déchiffre un entier
    */

    int decrypt(const int i) const;
    /**
     * Chiffre un tableau de bytes
     */
    std::vector<unsigned char> encryptBytes(const std::vector<unsigned char>& data) const;

    /**
     * Déchiffre un tableau de bytes
     */
    std::vector<unsigned char> decryptBytes(const std::vector<unsigned char>& encrypted) const;

    /**
     * Change la clé de chiffrement (plain)
     */
    void setKey(const std::string& newKey);

    /**
     * Change la clé de chiffrement avec encodage
     */
    void setKey(const std::string& newKey, KeyEncoding::Type encoding);

    /**
     * Obtient la longueur de la clé actuelle
     */
    size_t getKeyLength() const;

    /**
     * Encode la clé actuelle en Base64
     */
    std::string getKeyAsBase64() const;

    /**
     * Encode la clé actuelle en Base32
     */
    std::string getKeyAsBase32() const;

    /**
     * Méthodes statiques d'encodage/décodage
     */
    static std::string encodeBase64(const std::vector<unsigned char>& data);
    static std::string encodeBase32(const std::vector<unsigned char>& data);
    static std::string bytesToHex(const std::vector<unsigned char>& bytes);
    static std::vector<unsigned char> hexToBytes(const std::string& hex);
};

#endif // CIPHER_H