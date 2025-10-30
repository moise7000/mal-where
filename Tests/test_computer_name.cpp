//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include "../packer/ComputerName.h"

int main() {
    std::string computerName = getName();

    if (!computerName.empty()) {
        std::cout << "Nom de l'ordinateur : " << computerName << std::endl;
        std::cout << "Longueur du nom : " << computerName.length() << std::endl;
    } else {
        std::cout << "Erreur : impossible de recuperer le nom de l'ordinateur" << std::endl;
        return 1;
    }

    return 0;g
}