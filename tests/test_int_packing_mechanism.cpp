//
// Created by ewan decima on 10/18/25.
//

#include "../packer/Packer.h"
#include "../packer/Stub.h"
#include "TestingTools.h"
#include <iostream>
#include <iomanip>


std::ostream operator<<(const std::ostream & lhs, const std::vector<unsigned char> & rhs);

int main() {

    const std::string KEY = "LHS-PC";
    const Packer packer(KEY);
    const Stub stub(KEY);

    const int originalInt = 987654321;
    const std::vector<unsigned char> packedInt = packer.pack(originalInt);
    TestingTools::printVector("[DEBUG] Packed int:", packedInt);
    const int unpackedInt = stub.unpack(packedInt);
    std::cout << "[DEBUG] Unpacked int: " << unpackedInt << std::endl;
    const bool success = (originalInt == unpackedInt);

    if (success)
        TestingTools::printGreen("Test passed: pack/unpack int");
    else
        TestingTools::printRed("Test failed: pack/unpack int");



    return 0;
}
