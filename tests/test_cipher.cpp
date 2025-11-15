//
// Created by ewan decima on 10/18/25.
//

#include "../packer/Cipher.h"
#include "TestingTools.h"
#include <iostream>
#include <iomanip>


int main() {
    try {
        // Tableau de bytes original
        unsigned char code[] = {0x78, 0x9c, 0x0b, 0x33, 0x11, 0xaa, 0x98, 0xb5, 0xe7, 0xde, 0x07, 0x00, 0x0e, 0x30, 0x04, 0x39};
        size_t originalSize = sizeof(code);


        // Création du cipher avec une clé
        const std::string key = "MaSuperCleSecrete123";
        const Cipher cipher(key);

        std::vector<unsigned char> dataVec(code, code + originalSize);
        std::vector<unsigned char> encrypted = cipher.encryptBytes(dataVec);
        std::string hexEncrypted = Cipher::bytesToHex(encrypted);
        std::vector<unsigned char> decrypted = cipher.decryptBytes(encrypted);


        const bool success = (dataVec == decrypted);
        if (success) TestingTools::printGreen("Test 1 passed: bytes encryption"); else TestingTools::printRed("Test failed: bytes encryption");


        const int d = 1345;
        int const encryptedInt = cipher.encrypt(d);
        const bool temp = (d == encryptedInt);
        if (temp) TestingTools::printRed("Test 2 failed : int encryption"); else {
            int const decryptedInt = cipher.decrypt(encryptedInt);
            const bool secondSuccess = (d == decryptedInt);
            if (secondSuccess) TestingTools::printGreen("Test 2 passed: int encryption"); else TestingTools::printRed("Test failed: int encryption");
        }

        const std::string message = "printf";
        std::string encryptedString = cipher.encrypt(message);
        const bool temp2 = (message == encryptedString);
        if (temp2) TestingTools::printRed("Test 3 failed: string encryption"); else {
            std::string decryptedString = cipher.decrypt(encryptedString);
            const bool thirdSuccess = (message == decryptedString);
            if (thirdSuccess) TestingTools::printGreen("Test 3 passed: string encrytion/decryption"); else TestingTools::printRed("Test failed: string encryption/decryption");
        }





    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}