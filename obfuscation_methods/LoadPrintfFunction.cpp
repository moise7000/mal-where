#include "LoadPrintfFunction.h"
#include "../env/SystemEnvironment.h"
#include "../packer/Cipher.h"
#include <stdio.h>
#include <string>

std::string vectorToString(const std::vector<unsigned char> &vec) {
    if (vec.empty()) return std::string();
    return std::string(reinterpret_cast<const char*>(&vec[0]), vec.size());
}

// Fonction qui encapsule le chargement dynamique de printf
type_printf LoadPrintfFunction() {
    // Charger la bibliothèque msvcrt.dll dynamiquement
    HMODULE msvcrt = LoadLibraryA("msvcrt.dll");

    if (msvcrt == NULL) {
        fprintf(stderr, "Erreur lors du chargement de msvcrt.dll\n");
        return NULL;
    }


    const std::string key = systemEnvironment::getProcessorArchitecture();
    const Cipher cipher(key);
    unsigned char bytes[] = {0x0e, 0x6c, 0xe5, 0xd2, 0xe7, 0x5c};
    std::vector<unsigned char> encryptedBytes(bytes, bytes + 6);
    std::vector<unsigned char> decryptedBytes = cipher.decryptBytes(encryptedBytes);

    std::string arg = vectorToString(decryptedBytes); // = printf



    // Obtenir l'adresse de la fonction printf
    type_printf f = (type_printf)GetProcAddress(msvcrt, arg);

    if (f == NULL) {
        fprintf(stderr, "Erreur lors de la recuperation de printf\n");
        FreeLibrary(msvcrt);
        return NULL;
    }

    // Note: La bibliotheque reste chargee, il faudra la liberer manuellement
    return f;
}

// Fonction pour liberer la bibliotheque
void UnloadPrintfFunction() {
    HMODULE msvcrt = GetModuleHandleA("msvcrt.dll");
    if (msvcrt != NULL) {
        FreeLibrary(msvcrt);
    }
}

