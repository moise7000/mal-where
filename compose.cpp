#include <iostream>
#include <string>

/**
 * @brief Exécute une fonction f un nombre x de fois en chaînant le résultat.
 * 
 * Cette fonction implémente la composition f(f(...f(arg)...)) en utilisant 
 * la récursivité et les templates.
 * Elle suppose que le résultat de f(arg) a le même type que l'argument 'arg'
 * pour que la composition puisse continuer.
 * 
 * @tparam F Type de la fonction (pointeur de fonction ou foncteur).
 * @tparam T Type de l'argument initial.
 * @param f La fonction à appliquer.
 * @param x Le nombre d'appels à effectuer.
 * @param arg L'argument initial.
 * @return Le résultat final de la composition.
 */
template <typename F, typename T>
T compose_n_times(F f, int x, T arg) {
    // Cas de base : Si x est inférieur ou égal à 0, on s'arrête 
    // et on retourne l'argument 
    if (x <= 0) {
        return arg; 
    } else {
        // Étape récursive :
        // 1. Appelle la fonction f avec l'argument actuel.
        // 2. Passe le résultat de f à l'appel récursif suivant avec x-1.
        // La récursivité déroule les appels de l'intérieur vers l'extérieur.
        return compose_n_times(f, x - 1, f(arg));
    }
}