//
// Created by ewan decima on 11/18/25.
//

#include "LoadCheckRemoteDebuggerPresent.h"

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


    unsigned char bytes[] = {};

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