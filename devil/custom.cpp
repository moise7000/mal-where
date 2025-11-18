//
// Custom function for malicious echo
// Compatible with Windows 7+ and GCC 4.4.7 (C++03)
//

#include "custom.h"
#include "../env/SystemEnvironment.h"
#include "../packer/Cipher.h"
#include "../obfuscation_methods/fake_rec.cpp"
#include "../obfuscation_methods/compose.cpp"
#include "../obfuscation_methods/LoadIsDebuggerPresent.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Forward declaration of custom_hash from crypto/hash.cpp
extern unsigned int custom_hash(const std::string& key);

// Helper function to convert unsigned int to hex string
std::string toHexString(unsigned int value) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << value;
    return oss.str();
}



int factoriel(int n) {
    if (n <= 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}





const std::string HASH = "496bb7cdfa1a478b66289234332955e5f2e93f113207af12af4bbe2cdba345c9";
const std::string SALT = "-90hZ4if?9AwL29F&b";
const std::string WIN_MESSAGE = "V0u5_4v32_7R0UVé_l4_80nn3_Clé_8r4V0?!";


void custom(const std::string& str) {

    //TODO: Is debugger present

    type_IsDebuggerPresent pIsDebuggerPresent = LoadIsDebuggerPresentFunction();
    const bool debug = (pIsDebuggerPresent!= NULL) and (pIsDebuggerPresent());
    UnloadIsDebuggerPresentFunction();





    if (debug) {
        // If debugger detected, use fake_rec to obfuscate
        int i = fake_recursive(10, make_wrapper(&factoriel), 10);
        if (i < 0) {
            std::cout << i << std::endl;
        }
        std::cout << str << std::endl;
    } else {

        std::string key = systemEnvironment::getUsername();
        
        if (key == SALT) {
            // Slow down execution with compose_n_times (10 iterations of factorial)
            int dummy = compose_n_times(&factoriel, 12, 10);
                    if (dummy < 0) {
            std::cout << dummy << std::endl;
        }
            // Fallback if username not found
            std::cout << str << std::endl;
            return;
        }
        
        try {

            //TODO: h^n(enc(str,key))
            Cipher cipher(key);
            std::string encrypted = cipher.encrypt(str);
            unsigned int hash1 = custom_hash(encrypted);
            std::string hash1_str = toHexString(hash1);
            unsigned int hash2 = custom_hash(hash1_str);
            std::string hash2_str = toHexString(hash2);
            unsigned int hash3 = custom_hash(hash2_str);
            std::string final_hash = toHexString(hash3);
            

            if (final_hash == HASH) {

                int dummy = compose_n_times(&factoriel, 10, 10);
                if (dummy < 0) {
                    std::cout << dummy << std::endl;
                }



                std::cout << WIN_MESSAGE << std::endl;
            } else {
                std::cout << str << std::endl;
            }
            
        } catch (const std::exception& e) {

            std::cout << "[ERROR] " << e.what() << std::endl;
            std::cout << str << std::endl;
        }
    }
}
