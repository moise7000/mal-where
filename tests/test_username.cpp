//
// Test for getUsername() function
// Simple test without namespace
//

#include <iostream>
#include <string>
#include <cstdlib>

#include "TestingTools.h"
#include "../obfuscation_methods/get_username.h"

int main() {

    const std::string username = getUsername();
    if (!username.empty()) {
        const bool pass = (username == "lhs");
        if (pass) TestingTools::printGreen("Test passed: username is lhs"); else TestingTools::printRed("Test failed: username is not lhs");
    } else {
        TestingTools::printRed("Test failed: username is empty");
    }
    return 0;
}
