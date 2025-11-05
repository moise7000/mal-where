//
// Created by ewan decima on 11/5/25.
//

#include <iostream>
#include <ostream>

#include "TestingTools.h"
#include "../env/SystemEnvironment.h"

int main() {
    const std::string tmpPath = systemEnvironment::getTempPath();
    
    const bool passed = tmpPath ==  "C:\\Users\\lhs\\AppData\\Local\\Temp\\";

    if (passed) TestingTools::printGreen("Test passed: tmp path"); else TestingTools::printRed("Test failed");
}