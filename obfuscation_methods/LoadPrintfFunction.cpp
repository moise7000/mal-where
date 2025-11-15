#include "LoadPrintfFunction.h"
#include "../env/SystemEnvironment.h"
#include "../packer/Cipher.h"
#include <stdio.h>
#include <string>

#include <vector>


const char* vectorToConstChar(const std::vector<unsigned char> &vec,
                              std::string &buffer)
{
    if (vec.empty()) {
        buffer.clear();
        return buffer.c_str();
    }

    buffer.assign(reinterpret_cast<const char*>(&vec[0]), vec.size());
    return buffer.c_str();
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

    std::string temp;
    const char* cstr = vectorToConstChar(decryptedBytes, temp); //cstr = printf



    // Obtenir l'adresse de la fonction printf
    type_printf f = (type_printf)GetProcAddress(msvcrt, cstr);

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

