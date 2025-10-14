#include <iostream>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <iomanip>
#include <sstream>

using namespace std;

// Fonction pour convertir les bytes en hexadécimal
string bytesToHex(const unsigned char* data, int len) {
    stringstream ss;
    for (int i = 0; i < len; i++) {
        ss << hex << setw(2) << setfill('0') << (int)data[i];
    }
    return ss.str();
}

// Fonction pour afficher les erreurs OpenSSL
void printErrors() {
    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
        cerr << "Erreur OpenSSL: " << ERR_error_string(err, NULL) << endl;
    }
}

// Fonction de chiffrement AES-256-CBC
string encryptString(const string& plaintext, const string& password) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        cerr << "Erreur: impossible de créer le contexte de chiffrement" << endl;
        return "";
    }

    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char ciphertext[plaintext.length() + EVP_MAX_BLOCK_LENGTH];
    int ciphertext_len = 0;
    int len = 0;

    // Générer une clé à partir du mot de passe (avec du salt)
    unsigned char salt[8];
    if (!RAND_bytes(salt, sizeof(salt))) {
        cerr << "Erreur: impossible de générer le salt" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (!EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(),
                        salt, (unsigned char*)password.c_str(), password.length(),
                        1, key, iv)) {
        cerr << "Erreur: impossible de générer la clé" << endl;
        printErrors();
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Initialiser le contexte de chiffrement
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        cerr << "Erreur: impossible d'initialiser le chiffrement" << endl;
        printErrors();
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // Chiffrer les données
    if (!EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char*)plaintext.c_str(), plaintext.length())) {
        cerr << "Erreur: échec du chiffrement" << endl;
        printErrors();
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len = len;

    // Finaliser le chiffrement
    if (!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        cerr << "Erreur: échec de la finalisation du chiffrement" << endl;
        printErrors();
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len += len;

    // Libérer le contexte
    EVP_CIPHER_CTX_free(ctx);

    // Combiner salt + texte chiffré et retourner en hexadécimal
    string result = "Salted__" + bytesToHex(salt, sizeof(salt)) + bytesToHex(ciphertext, ciphertext_len);
    return result;
}

int main(int argc, char* argv[]) {
    // Initialiser OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <texte_a_chiffrer> [mot_de_passe]" << endl;
        cout << "Exemple: " << argv[0] << " \"Bonjour le monde\" \"ma_cle_secrete\"" << endl;
        return 1;
    }

    string plaintext = argv[1];
    string password = (argc > 2) ? argv[2] : "password123";

    cout << "Texte original: " << plaintext << endl;
    cout << "Mot de passe: " << password << endl;

    string encrypted = encryptString(plaintext, password);

    if (!encrypted.empty()) {
        cout << "Texte chiffré (hex): " << encrypted << endl;
    } else {
        cerr << "Erreur: le chiffrement a échoué" << endl;
        return 1;
    }

    // Nettoyer OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
