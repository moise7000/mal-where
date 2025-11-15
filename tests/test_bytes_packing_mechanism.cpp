//
// Created by ewan decima on 11/15/25.
//

#include "../packer/Packer.h"
#include "../packer/Stub.h"
#include "TestingTools.h"
#include <iostream>
#include <vector>
#include <iomanip>


std::ostream operator<<(const std::ostream & lhs, const std::vector<unsigned char> & rhs);

bool operator==(int lhs, const std::vector<unsigned char> & rhs);

int main() {

    const std::string KEY = "LHS-PC";
    const Packer packer(KEY);
    const Stub stub(KEY);

    std::vector<unsigned char> originalBytes = {0x70, 0x72, 0x69, 0x6E, 0x74, 0x66};
    size_t originalSize = originalBytes.size();



    const std::vector<unsigned char> packedBytes = packer.pack(originalBytes, originalSize);
    TestingTools::printVector("[DEBUG] Packed bytes:", packedBytes);
    const std::string unpackedString = stub.unpackToString(packedBytes, originalSize);
    std::cout << unpackedString << std::endl;
    const bool success = (unpackedString == "printf");

    if (success)
        TestingTools::printGreen("Test passed: pack/unpack int");
    else
        TestingTools::printRed("Test failed: pack/unpack int");



    return 0;
}
