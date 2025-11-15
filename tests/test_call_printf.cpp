//
// Created by ewan decima on 11/15/25.
//


#include "../obfuscation_methods/LoadPrintfFunction.h"

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