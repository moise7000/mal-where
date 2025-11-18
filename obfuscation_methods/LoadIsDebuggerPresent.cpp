//
// Created by ewan decima on 11/18/25.
//

#include "LoadIsDebuggerPresent.h"
#include <Windows.h>
#include <stdio.h>
#include "../packer/Cipher.h"
#include "../env/SystemEnvironment.h"

// Fonction qui encapsule le chargement dynamique de IsDebuggerPresent
type_IsDebuggerPresent LoadIsDebuggerPresentFunction() {

    HMODULE kernel32 = LoadLibraryA("kernel32.dll");

    if (kernel32 == NULL) {
        //fprintf(stderr, "Erreur lors du chargement de kernel32.dll\n");
        return NULL;
    }


    const std::string key = systemEnvironment::getUsername();
    const Cipher cipher(key);


    unsigned char bytes[] = {0x29, 0x32, 0xde, 0x0e, 0x2e,0xe0, 0x11, 0x30, 0xe5, 0x03,0x02, 0xf9, 0x1c, 0x1f, 0xe6,0x35, 0x1d, 0xd2, 0x27, 0x0d, 0xc8,0xf9, 0x4b, 0x2e,0x38, 0x05};

    std::vector<unsigned char> encryptedBytes(bytes, bytes + sizeof(bytes));
    std::vector<unsigned char> decryptedBytes = cipher.decryptBytes(encryptedBytes);
    std::string functionName(decryptedBytes.begin(), decryptedBytes.end());

    type_IsDebuggerPresent func = (type_IsDebuggerPresent)GetProcAddress(
        kernel32,
        functionName.c_str()
    );


    if (func == NULL) {
        //fprintf(stderr, "Erreur lors de la recuperation de IsDebuggerPresent\n");
        FreeLibrary(kernel32);
        return NULL;
    }


    return func;
}

// Fonction pour libérer la bibliothèque
void UnloadIsDebuggerPresentFunction() {
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (kernel32 != NULL) {
        FreeLibrary(kernel32);
    }
}