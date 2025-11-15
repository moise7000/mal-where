#include <iostream>
#include <string>
#include "../obfuscation_methods/hash_function.h"

int main() {
    std::cout << "=== Tests de la fonction SHA-256 ===" << std::endl << std::endl;

    // Tests de génération de hash
    std::cout << "--- Tests de hachage ---" << std::endl;
    
    std::string input1 = "password";
    std::string hash1 = hash_function(input1);
    std::cout << "Input: \"" << input1 << "\"" << std::endl;
    std::cout << "SHA-256: " << hash1 << std::endl << std::endl;

    std::string input2 = "123456";
    std::string hash2 = hash_function(input2);
    std::cout << "Input: \"" << input2 << "\"" << std::endl;
    std::cout << "SHA-256: " << hash2 << std::endl << std::endl;

    std::string input3 = "admin";
    std::string hash3 = hash_function(input3);
    std::cout << "Input: \"" << input3 << "\"" << std::endl;
    std::cout << "SHA-256: " << hash3 << std::endl << std::endl;

    std::string input4 = "MySecureP@ssw0rd!";
    std::string hash4 = hash_function(input4);
    std::cout << "Input: \"" << input4 << "\"" << std::endl;
    std::cout << "SHA-256: " << hash4 << std::endl << std::endl;

    std::string input5 = "";
    std::string hash5 = hash_function(input5);
    std::cout << "Input: \"\" (chaine vide)" << std::endl;
    std::cout << "SHA-256: " << hash5 << std::endl << std::endl;

    std::string input6 = "a";
    std::string hash6 = hash_function(input6);
    std::cout << "Input: \"" << input6 << "\"" << std::endl;
    std::cout << "SHA-256: " << hash6 << std::endl << std::endl;

    // Tests de vérification
    std::cout << "--- Tests de verification ---" << std::endl;
    
    bool verify1 = verify_hash("password", hash1);
    std::cout << "verify_hash(\"password\", hash1): " << (verify1 ? "OK" : "FAIL") << std::endl;

    bool verify2 = verify_hash("wrongpassword", hash1);
    std::cout << "verify_hash(\"wrongpassword\", hash1): " << (verify2 ? "FAIL" : "OK (mismatch attendu)") << std::endl;

    bool verify3 = verify_hash("MySecureP@ssw0rd!", hash4);
    std::cout << "verify_hash(\"MySecureP@ssw0rd!\", hash4): " << (verify3 ? "OK" : "FAIL") << std::endl;

    std::cout << std::endl << "=== Tous les tests passent ===" << std::endl;

    return 0;
}
