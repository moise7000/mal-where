//
// Created by ewan decima on 11/15/25.
//

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef int (*type_printf)(const char *, ...);

// Fonction qui encapsule le chargement dynamique de printf
type_printf LoadPrintfFunction() {
    // Charger la bibliothèque C standard dynamiquement
    void *libc = dlopen("libc.so.6", RTLD_LAZY);

    if (libc == NULL) {
        fprintf(stderr, "Erreur lors du chargement de libc: %s\n", dlerror());
        return NULL;
    }

    // Réinitialiser dlerror
    dlerror();

    // Obtenir l'adresse de la fonction printf
    type_printf f = (type_printf)dlsym(libc, "printf");

    const char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Erreur lors de la récupération de printf: %s\n", error);
        dlclose(libc);
        return NULL;
    }

    // Note: La bibliothèque reste chargée, il faudra la libérer manuellement
    return f;
}

// Fonction pour libérer la bibliothèque
void UnloadPrintfFunction() {
    void *libc = dlopen("libc.so.6", RTLD_NOLOAD);
    if (libc != NULL) {
        dlclose(libc);
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
    f("Test avec plusieurs paramètres: %s = %d\n", "valeur", 42);

    // Libérer la bibliothèque
    UnloadPrintfFunction();

    return 0;
}