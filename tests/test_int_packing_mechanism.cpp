//
// Created by ewan decima on 10/18/25.
//

#include "../packer/Packer.h"
#include "../packer/Stub.h"
#include "TestingTools.h"
#include <iostream>
#include <iomanip>


int main() {

    const std::string KEY = "LHS-PC";
    Packer packer(KEY);
    Stub stub(KEY);

    const int originalInt = 987654321;
    const std::vector<unsigned char> packedInt = packer.pack(originalInt);
    const int unpackedInt = stub.unpack(packedInt);
    const bool success = (originalInt == unpackedInt);

    if (success)
        TestingTools::printGreen("Test passed: pack/unpack int");
    else
        TestingTools::printRed("Test failed: pack/unpack int");



    return 0;
}