#include "LoadPrintfFunction.h"
#include "../env/SystemEnvironment.h"
#include "../packer/Cipher.h"
#include <stdio.h>
#include <string>

#include <vector>





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
    unsigned char bytes[] = {0xb0, 0xc1, 0xc9, 0xa5, 0xca, 0xc9};
    std::vector<unsigned char> encryptedBytes(bytes, bytes + 6);
    std::vector<unsigned char> decryptedBytes = cipher.decryptBytes(encryptedBytes);
    const std::string functionName(decryptedBytes.begin(), decryptedBytes.end());

    type_printf f = (type_printf)GetProcAddress(msvcrt, functionName.c_str());

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

