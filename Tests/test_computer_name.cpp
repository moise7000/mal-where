//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include "../packer/ComputerName.h"
#include "TestingTools.h"

int main() {
    std::string computerName = getName();

    if (!computerName.empty()) {
        std::cout << "Computer's name: " << computerName << std::endl;
        if (computerName == "LHS-PC") TestingTools::printGreen("Test passed"); else TestingTools::printGreen("Test failed");
    } else {
        TestingTools::printRed("Error: Unable to retrieve computer name");
        return 1;
    }

    return 0;
}