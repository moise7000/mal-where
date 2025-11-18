//
// Created by ewan decima on 11/16/25.
//

#include <iostream>
#include <ostream>

#include "devil/DevilCode.h"
#include "devil/MaliciousEcho.h"
#include "devil/custom.h"

int main(int argc, char* argv[]) {
    // Vérifier qu'un argument a été fourni
    if (argc < 2) {
        std::cerr << "Usage : " << argv[0] << " <chaîne>" << std::endl;
        return 1;
    }


    std::string input = argv[1];


   return  malicious_echo(input, devil, custom);

    return 0;
}