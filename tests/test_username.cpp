//
// Test for getUsername() function
// Simple test without namespace
//

#include <iostream>
#include <string>
#include <cstdlib>
#include "../obfuscation_methods/get_username.h"

int main() {
    std::cout << "  Test getUsername() - Windows 7+    " << std::endl;
    std::cout << std::endl;

    // Get username
    std::string username = getUsername();

    if (!username.empty()) {
        std::cout << "USERNAME = \"" << username << "\"" << std::endl;
        std::cout << "[OK] Username retrieved" << std::endl;
    } else {
        std::cout << "USERNAME = (empty)" << std::endl;
        std::cout << "[FAIL] Could not retrieve username" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "            Test Complete             " << std::endl;


    return 0;
}
