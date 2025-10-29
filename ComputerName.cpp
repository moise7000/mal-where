//
// Created by ewan decima on 10/29/25.
//

#include <Windows.h>
#include "ComputerName.h"

/** |------------------------------------------------------|
*   |        Fonction qui renvoie a priori "LHS-PC"        |
*/  |------------------------------------------------------|


std::string getName() {
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer);

    if (GetComputerNameA(buffer, &size)) {
        return std::string(buffer);
    }

    return "";
}

