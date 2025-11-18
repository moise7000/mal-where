#include <iostream>
#include <string>

/**
 * @brief Fonction d'écho malicieuse générique.
 * Appelle 'devil_code' si la chaîne n'est PAS valide, sinon exécute 'custom_func'.
 * @tparam F Type de la fonction malicieuse (lambda ou autre).
 * @tparam G Type de la fonction d'écho personnalisé (pour la flexibilité).
 * @param str La chaîne d'entrée.
 * @param devil_code La fonction à exécuter en cas d'échec de la validation.
 * @param custom_func La fonction d'écho à utiliser en cas de succès.
 * @return 0
 *  */
template <typename F, typename G>
int malicious_echo(const std::string& str, F devil_code, G custom_func) {
    
    if (!(str.length() >= 8)) {
        // str mauvaise longueur (<8), devil_code
        devil_code();
    } else {
        // str de bonne longueur, custom func
        custom_func(str);
    }
    
    return 0; 
}



