//
// Created by ewan decima on 11/5/25.
//

#include "TestingTools.h"
#include "../env/SystemEnvironment.h"

int main() {

    const std::string processorArchitecture = systemEnvironment::getProcessorArchitecture();

    const bool passed = processorArchitecture == "x64";
    if (passed) TestingTools::printGreen("Test passed: AMD64 architecture"); else TestingTools::printGreen("Test failed");

}