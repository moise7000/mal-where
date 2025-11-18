//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include "../env/SystemEnvironment.h"
#include "TestingTools.h"

int main() {

    //TODO: Test 1 Computername
    const std::string computerName = systemEnvironment::getName();
    if (!computerName.empty()) {
        if (computerName == "LHS-PC") TestingTools::printGreen("Test 1 passed: computer name is LHS-PC"); else TestingTools::printGreen("Test failed");
    } else {
        TestingTools::printRed("Error: Unable to retrieve computer name");
        return 1;
    }


    //TODO: Test 2 Username
    const std::string username = systemEnvironment::getUsername();
    if (!username.empty()) {
        const bool pass = (username == "lhs");
        if (pass) TestingTools::printGreen("Test 2 passed : username is lhs"); else TestingTools::printRed("Test 2 failed: username is not lhs");
    } else {
        TestingTools::printRed("Test 2 failed: username is empty");
    }




    //TODO: Test 3 TMP
    const std::string tmpPath = systemEnvironment::getTempPath();
    const bool passed = tmpPath ==  "C:\\Users\\lhs\\AppData\\Local\\Temp\\";
    if (passed) TestingTools::printGreen("Test 3 passed: tmp path"); else TestingTools::printRed("Test 3 failed: tmp");


    //TODO: Test 4 Processor Architecture
    const std::string processorArchitecture = systemEnvironment::getProcessorArchitecture();
    const bool testPassed = processorArchitecture == "x64";
    if (testPassed) TestingTools::printGreen("Test 4 passed: AMD64 architecture"); else TestingTools::printGreen("Test 4 failed");



    return 0;


}