//
// Created by ewan decima on 10/13/25.
//

#ifndef MAL_WHERE_SYMMETRIC_CIPHER_H
#define MAL_WHERE_SYMMETRIC_CIPHER_H
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>


class SymmetricCipher {

private:
    static const int IV_SIZE = 16;
    static const int SALT_SIZE = 8;

    // Dérive une clé à partir d'une string en utilisant PBKDF2
    std::string deriveKey(const std::string& password,
                          const unsigned char* salt,
                          int keySize) const {
        unsigned char* key[EVP_MAX_KEY_LENGTH];

        PKCS5_PBKDF2_HMAC(
            password.c_str(),
            password.length(),
            salt,
            SALT_SIZE,
            10000,
            EVP_sha256(),
            keySize,
            key);

        return std::string(reinterpret_cast<char*>(key),keySize);
    }

public:
    SymmetricCipher() {
        
    }



};


#endif //MAL_WHERE_SYMMETRIC_CIPHER_H