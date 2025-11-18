// Test custom function with malicious echo
// Compatible GCC 4.4.7 / Windows 7+
//

#include <iostream>
#include <string>
#include "../devil/MaliciousEcho.h"
#include "../devil/custom.h"
#include "../devil/DevilCode.h"

// Wrapper pour devil code
void devil_wrapper(const std::string& s) {
    std::cout << "[DEVIL TRIGGERED] String too short: " << s << std::endl;
    // Uncomment to actually trigger devil code:
    // devil();
}

int main() {

    
    std::cout << "[TEST 1] String too short (< 8 chars) - triggers devil" << std::endl;
    malicious_echo("short", devil_wrapper, custom);
    
    std::cout << "\n[TEST 2] Valid string (>= 8 chars) - uses custom function" << std::endl;
    malicious_echo("valid_string_test", devil_wrapper, custom);
    
    std::cout << "\n[TEST 3] Another valid string" << std::endl;
    malicious_echo("hello world from malware", devil_wrapper, custom);
    
    std::cout << "\n[TEST 4] Try to find the secret message..." << std::endl;
    std::cout << "Hint: Encrypt a string with your USERNAME, triple-hash it" << std::endl;
    std::cout << "and check if it equals 'V0u5_4v32_7R0UVé_l4_80nn3_Clé_8r4V0?!'" << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "   TESTS COMPLETED                      " << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
