#include <windows.h>
#include <stdio.h>

typedef int (*type_printf)(const char *, ...);

// Fonction qui encapsule le chargement dynamique de printf
type_printf LoadPrintfFunction() {
    // Charger la bibliothèque msvcrt.dll dynamiquement
    HMODULE msvcrt = LoadLibraryA("msvcrt.dll");

    if (msvcrt == NULL) {
        fprintf(stderr, "Erreur lors du chargement de msvcrt.dll\n");
        return NULL;
    }

    // Obtenir l'adresse de la fonction printf
    type_printf f = (type_printf)GetProcAddress(msvcrt, "printf");

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

int main(int argc, char *argv[]) {
    // Charger la fonction printf dynamiquement
    type_printf f = LoadPrintfFunction();

    if (f == NULL) {
        return 1;
    }

    // Utiliser la fonction printf
    f("Hello world\n");
    f("Nombre d'arguments: %d\n", argc);
    f("Test avec plusieurs parametres: %s = %d\n", "valeur", 42);

    // Liberer la bibliotheque
    UnloadPrintfFunction();

    return 0;
}