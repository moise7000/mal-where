#include "Cipher.h"
#include <algorithm>
#include <sstream>

// Initialisation des constantes statiques
const std::string Cipher::BASE64_CHARS = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const std::string Cipher::BASE32_CHARS = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Constructeur avec clé personnalisée (plain)
Cipher::Cipher(const std::string& customKey) : key(customKey) {
    if (customKey.empty()) {
        throw std::runtime_error("[ERROR] The key cannot be empty");
    }
}

// Constructeur avec clé personnalisée et encodage
Cipher::Cipher(const std::string& customKey, KeyEncoding::Type encoding) {
    if (customKey.empty()) {
        throw std::runtime_error("[ERROR] The key cannot be empty");
    }

    // Décode la clé selon l'encodage spécifié
    switch (encoding) {
        case KeyEncoding::BASE64:
            key = decodeBase64(customKey);
            break;
        case KeyEncoding::BASE32:
            key = decodeBase32(customKey);
            break;
        case KeyEncoding::PLAIN:
        default:
            key = customKey;
            break;
    }

    if (key.empty()) {
        throw std::runtime_error("[ERROR] Decoded key is empty - invalid encoding");
    }
}

// Destructeur
Cipher::~Cipher() {
    // Nettoyage si nécessaire
}

// Décode une chaîne Base64
std::string Cipher::decodeBase64(const std::string& encoded) {
    std::string decoded;
    std::vector<int> T(256, -1);

    for (size_t i = 0; i < 64; i++) {
        T[static_cast<unsigned char>(BASE64_CHARS[i])] = static_cast<int>(i);
    }

    int val = 0;
    int valb = -8;
    
    for (size_t i = 0; i < encoded.length(); i++) {
        unsigned char c = static_cast<unsigned char>(encoded[i]);
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

// Décode une chaîne Base32
std::string Cipher::decodeBase32(const std::string& encoded) {
    std::string decoded;
    std::vector<int> T(256, -1);

    for (size_t i = 0; i < 32; i++) {
        T[static_cast<unsigned char>(BASE32_CHARS[i])] = static_cast<int>(i);
    }

    int val = 0;
    int valb = -5;
    
    for (size_t i = 0; i < encoded.length(); i++) {
        unsigned char c = static_cast<unsigned char>(encoded[i]);
        if (c == '=') break; // Padding
        if (T[c] == -1) continue;

        val = (val << 5) + T[c];
        valb += 5;
        if (valb >= 0) {
            decoded.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

// Génère une clé étendue basée sur SHA-256 simplifié
std::vector<unsigned char> Cipher::generateExtendedKey(size_t length) const {
    std::vector<unsigned char> extendedKey;
    extendedKey.reserve(length);

    size_t keyLen = key.length();
    if (keyLen == 0) {
        throw std::runtime_error("[ERROR] The key cannot be empty");
    }

    // Génération d'une clé étendue avec transformation
    for (size_t i = 0; i < length; ++i) {
        unsigned char base = static_cast<unsigned char>(key[i % keyLen]);
        unsigned char transform = static_cast<unsigned char>((i / keyLen) & 0xFF);

        // Mélange simple mais efficace
        unsigned char mixed = base ^ transform;
        mixed = ((mixed << 3) | (mixed >> 5)) & 0xFF;
        mixed ^= static_cast<unsigned char>((i ^ keyLen) & 0xFF);

        extendedKey.push_back(mixed);
    }

    return extendedKey;
}

// Chiffre un message texte
std::string Cipher::encrypt(const std::string& message) const {
    std::vector<unsigned char> data(message.begin(), message.end());
    std::vector<unsigned char> encrypted = encryptBytes(data);
    return std::string(encrypted.begin(), encrypted.end());
}

// Déchiffre un message texte
std::string Cipher::decrypt(const std::string& encrypted) const {
    std::vector<unsigned char> data(encrypted.begin(), encrypted.end());
    std::vector<unsigned char> decrypted = decryptBytes(data);
    return std::string(decrypted.begin(), decrypted.end());
}

// Chiffre un tableau de bytes
std::vector<unsigned char> Cipher::encryptBytes(const std::vector<unsigned char>& data) const {
    if (data.empty()) {
        return std::vector<unsigned char>();
    }

    std::vector<unsigned char> extKey = generateExtendedKey(data.size());
    std::vector<unsigned char> result(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ extKey[i];
    }

    return result;
}

// Déchiffre un tableau de bytes
std::vector<unsigned char> Cipher::decryptBytes(const std::vector<unsigned char>& encrypted) const {
    // XOR est symétrique, donc déchiffrement = chiffrement
    return encryptBytes(encrypted);
}

// Change la clé de chiffrement (plain)
void Cipher::setKey(const std::string& newKey) {
    if (newKey.empty()) {
        throw std::runtime_error("[ERROR] The key cannot be empty");
    }
    key = newKey;
}

// Change la clé de chiffrement avec encodage
void Cipher::setKey(const std::string& newKey, KeyEncoding::Type encoding) {
    if (newKey.empty()) {
        throw std::runtime_error("[ERROR] The key cannot be empty");
    }

    // Décode la nouvelle clé selon l'encodage spécifié
    switch (encoding) {
        case KeyEncoding::BASE64:
            key = decodeBase64(newKey);
            break;
        case KeyEncoding::BASE32:
            key = decodeBase32(newKey);
            break;
        case KeyEncoding::PLAIN:
        default:
            key = newKey;
            break;
    }

    if (key.empty()) {
        throw std::runtime_error("[ERROR] Decoded key is empty - invalid encoding");
    }
}

// Obtient la longueur de la clé actuelle
size_t Cipher::getKeyLength() const {
    return key.length();
}

// Encode la clé actuelle en Base64
std::string Cipher::getKeyAsBase64() const {
    return encodeBase64(std::vector<unsigned char>(key.begin(), key.end()));
}

// Encode la clé actuelle en Base32
std::string Cipher::getKeyAsBase32() const {
    return encodeBase32(std::vector<unsigned char>(key.begin(), key.end()));
}

// Encode bytes en Base64
std::string Cipher::encodeBase64(const std::vector<unsigned char>& data) {
    std::string encoded;
    int val = 0;
    int valb = -6;

    for (size_t i = 0; i < data.size(); i++) {
        unsigned char c = data[i];
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encoded.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

// Encode bytes en Base32
std::string Cipher::encodeBase32(const std::vector<unsigned char>& data) {
    std::string encoded;
    int val = 0;
    int valb = -5;

    for (size_t i = 0; i < data.size(); i++) {
        unsigned char c = data[i];
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(BASE32_CHARS[(val >> valb) & 0x1F]);
            valb -= 5;
        }
    }

    if (valb > -5) {
        encoded.push_back(BASE32_CHARS[((val << 8) >> (valb + 8)) & 0x1F]);
    }

    while (encoded.size() % 8) {
        encoded.push_back('=');
    }

    return encoded;
}

// Convertit bytes en hexadécimal
std::string Cipher::bytesToHex(const std::vector<unsigned char>& bytes) {
    const char hex[] = "0123456789abcdef";
    std::string result;
    result.reserve(bytes.size() * 2);

    for (size_t i = 0; i < bytes.size(); ++i) {
        unsigned char c = bytes[i];
        result.push_back(hex[(c >> 4) & 0x0F]);
        result.push_back(hex[c & 0x0F]);
    }

    return result;
}

// Convertit hexadécimal en bytes
std::vector<unsigned char> Cipher::hexToBytes(const std::string& hex) {
    if (hex.length() % 2 != 0) {
        throw std::runtime_error("[ERROR] Invalid hexadecimal string");
    }

    std::vector<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2) {
        char h1 = hex[i];
        char h2 = hex[i + 1];

        unsigned char b1 = (h1 >= '0' && h1 <= '9') ? h1 - '0' :
                          (h1 >= 'a' && h1 <= 'f') ? h1 - 'a' + 10 :
                          (h1 >= 'A' && h1 <= 'F') ? h1 - 'A' + 10 : 0;

        unsigned char b2 = (h2 >= '0' && h2 <= '9') ? h2 - '0' :
                          (h2 >= 'a' && h2 <= 'f') ? h2 - 'a' + 10 :
                          (h2 >= 'A' && h2 <= 'F') ? h2 - 'A' + 10 : 0;

        bytes.push_back((b1 << 4) | b2);
    }

    return bytes;
}