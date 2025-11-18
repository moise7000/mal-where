#include <iostream>
#include <string>
#include <cstdarg> // Pour les arguments variables

// Ma version simplifiée de printf qui imprime la chaîne fournie comme premier argument variable
// sans analyser les spécificateurs dans le 'format'.
void my_print_only_string(const char* format_placeholder, ...) {
    va_list args; // Déclare la liste d'arguments
    va_start(args, format_placeholder); // Initialise la liste, 'format_placeholder' est le dernier argument fixe

    // Nous supposons ici que le premier argument variable est LA chaîne que nous voulons imprimer.
    // Le 'format_placeholder' est simplement utilisé pour le mécanisme va_start,
    // nous ne l'analysons pas pour trouver des %s, %d, etc.
    const char* str_to_print = va_arg(args, const char*); // Récupère l'argument comme const char*

    if (str_to_print != NULL) { // Vérification simple pour éviter un crash si NULL est passé
        std::cout << str_to_print; // Imprime la chaîne
    }

    va_end(args); // Nettoie la liste d'arguments
}

