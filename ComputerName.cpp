//
// Created by ewan decima on 10/29/25.
//

#include <iostream>
#include <string>
#include <Windows.h>

/** |------------------------------------------------------|
*   |        Fonction qui renvoie a priori "LHS-PC"        |
*/  |------------------------------------------------------|

std::string GetComputerName() {
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer);
    if (!GetComputerNameA(buffer, &size)) {
        return std::string(buffer);
    } else {
        return "";
    }
}