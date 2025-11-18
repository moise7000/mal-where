//
// Custom function for malicious echo
// Compatible with Windows 7+ and GCC 4.4.7 (C++03)
//

#include "custom.h"
#include "../obfuscation_methods/get_username.h"
#include "../packer/Cipher.h"
#ifdef _WIN32
#include "../obfuscation_methods/anti_debug/anti_debug.h"
#endif
#include "../obfuscation_methods/fake_rec.cpp"
#include <iostream>
#include <sstream>
#include <iomanip>

// Forward declaration of custom_hash from crypto/hash.cpp
extern unsigned int custom_hash(const std::string& key);

// Factorial function for fake_rec
int factoriel(int n) {
    if (n <= 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Helper function to convert unsigned int to hex string
std::string toHexString(unsigned int value) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << value;
    return oss.str();
}

void custom(const std::string& str) {
    // Check if debugger is present
#ifdef _WIN32
    bool debug = anti_debug::is_debugger_present();
#else
    bool debug = false;  // No debugger detection on Linux in this version
#endif
    
    if (debug) {
        // If debugger detected, use fake_rec to obfuscate
        int i = fake_recursive(10, make_wrapper(&factoriel), 10);
        if (i < 0) {
            std::cout << i << std::endl;
        }
        std::cout << str << std::endl;
    } else {
        // Get the username as key
        std::string key = getUsername();
        
        if (key.empty()) {
            // Fallback if username not found
            std::cout << str << std::endl;
            return;
        }
        
        try {
            // Encrypt the string with username as key
            Cipher cipher(key);
            std::string encrypted = cipher.encrypt(str);
            
            // Apply triple hash
            unsigned int hash1 = custom_hash(encrypted);
            std::string hash1_str = toHexString(hash1);
            
            unsigned int hash2 = custom_hash(hash1_str);
            std::string hash2_str = toHexString(hash2);
            
            unsigned int hash3 = custom_hash(hash2_str);
            std::string final_hash = toHexString(hash3);
            
            // Check if it matches the secret
            if (final_hash == "V0u5_4v32_7R0UVé_l4_80nn3_Clé_8r4V0?!") {
                std::cout << "Vous avez gagne!" << std::endl;
            } else {
                // Display the original string
                std::cout << str << std::endl;
            }
            
        } catch (const std::exception& e) {
            // If encryption fails, just display the string
            std::cout << "[ERROR] " << e.what() << std::endl;
            std::cout << str << std::endl;
        }
    }
}
