#include <iostream>
#include <string>
#include <functional>
#include <utility>

// =================================================================
// 1. DÉPENDANCES SIMULÉES (Simule des fonctions qui viendraient d'autres fichiers)
// =================================================================

/**
 * @brief Namespace pour les fonctions d'Entrée/Sortie personnalisées.
 */
namespace CustomIO {
    
    /**
     * @brief Implémentation simulée de votre fonction d'écho maison.
     * Utiliserait des méthodes d'obfuscation ou un printf maison.
     */
    void custom_echo(const std::string& str) {
        std::cout << "[CUSTOM_ECHO]: " << str << " (Longueur: " << str.length() << " octets)" << std::endl;
    }
}

/**
 * @brief Namespace pour les fonctions de validation/sécurité.
 */
namespace SecurityUtils {
    
    /**
     * @brief Fonction de validation de la longueur (Simulée).
     * @param str La chaîne à valider.
     * @return true si la longueur est valide (>= 8), false sinon.
     */
    bool len_validation(const std::string& str) {
        // Exigence : 8 octets minimum
        return str.length() >= 8;
    }
}

// =================================================================
// 2. LA FONCTION CIBLE (Dans son propre namespace pour l'organisation)
// =================================================================

/**
 * @brief Namespace principal pour cette tâche.
 */
namespace Echo {

    // On utilise les dépendances avec leur namespace complet
    using namespace SecurityUtils;
    using namespace CustomIO;

    /**
     * @brief Fonction d'écho malicieuse générique.
     * Appelle 'devil_code' si la chaîne n'est PAS valide, sinon exécute 'custom_func'.
     * * @tparam F Type de la fonction malicieuse (lambda ou autre).
     * @tparam G Type de la fonction d'écho personnalisé (pour la flexibilité).
     * @param str La chaîne d'entrée.
     * @param devil_code La fonction à exécuter en cas d'échec de la validation.
     * @param custom_func La fonction d'écho à utiliser en cas de succès.
     * @return 0 (selon le prototype souhaité).
     */
    template <typename F, typename G>
    int malicious_echo(const std::string& str, F&& devil_code, G&& custom_func) {
        
        std::cout << "-> Tentative d'écho pour la chaîne : \"" << str << "\"" << std::endl;

        if (!len_validation(str)) {
            // Le test de validation ÉCHOUÉ : exécute le code "malicieux"
            std::cout << "   [ATTENTION]: La validation de longueur a ÉCHOUÉ (longueur < 8)." << std::endl;
            std::cout << "   [EXECUTION]: Appel de 'devil_code'..." << std::endl;
            std::forward<F>(devil_code)(str);
        } else {
            // Le test de validation RÉUSSIT : exécute la fonction d'écho normale
            std::cout << "   [SUCCÈS]: La validation de longueur a réussi." << std::endl;
            std::cout << "   [EXECUTION]: Appel de 'custom_func'..." << std::endl;
            std::forward<G>(custom_func)(str);
        }
        
        return 0; // Retourne 0 comme spécifié dans la signature
    }

    /**
     * @brief Surcharge pour fournir 'custom_echo' par défaut (plus proche de votre prototype).
     * Utilise le `std::function` par défaut du C++ (pointeur vers CustomIO::custom_echo).
     * Le type par défaut doit correspondre à la signature de la fonction.
     */
    template <typename F>
    int malicious_echo(const std::string& str, F&& devil_code) {
        // Appelle la version complète avec la fonction par défaut
        return malicious_echo(
            str, 
            std::forward<F>(devil_code), 
            CustomIO::custom_echo
        );
    }

} // namespace Echo

// =================================================================
// 3. UTILISATION ET TESTS (main)
// =================================================================

int main() {
    using namespace Echo;

    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         SUITE DE TESTS - MALICIOUS_ECHO FUNCTION              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    // ─────────────────────────────────────────────────────────────────
    // DÉFINITION DES FONCTIONS DE TEST
    // ─────────────────────────────────────────────────────────────────

    // 1. Code malicieux basique
    auto devil_code_basic = [](const std::string& failed_str) {
        std::cout << "     DEVIL CODE BASIQUE : Chaîne rejetée = \"" << failed_str << "\"" << std::endl;
        std::cout << "    Simulation : Enregistrement dans log malveillant..." << std::endl;
    };

    // 2. Code malicieux avancé avec compteur
    int malicious_counter = 0;
    auto devil_code_counter = [&malicious_counter](const std::string& failed_str) {
        malicious_counter++;
        std::cout << "     DEVIL CODE #" << malicious_counter 
                  << " : Tentative échouée avec \"" << failed_str << "\"" << std::endl;
        std::cout << "    Simulation : Exfiltration de données (tentative #" 
                  << malicious_counter << ")..." << std::endl;
    };

    // 3. Code malicieux qui analyse la chaîne
    auto devil_code_analyzer = [](const std::string& failed_str) {
        std::cout << "     DEVIL CODE ANALYZER : Analyse de la chaîne rejetée" << std::endl;
        std::cout << "    Longueur: " << failed_str.length() << " octets" << std::endl;
        std::cout << "    Première lettre: " << (failed_str.empty() ? "N/A" : std::string(1, failed_str[0])) << std::endl;
        std::cout << "    Simulation : Collecte de métadonnées pour attaque ciblée..." << std::endl;
    };

    // 4. Fonction d'écho en MAJUSCULES
    auto upper_case_echo = [](const std::string& str) {
        std::string upper_str = str;
        std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
        std::cout << "    [UPPER_ECHO]: " << upper_str << std::endl;
    };

    // 5. Fonction d'écho avec encadrement
    auto boxed_echo = [](const std::string& str) {
        std::cout << "   +" << std::string(str.length() + 2, '-') << "+" << std::endl;
        std::cout << "   | " << str << " |" << std::endl;
        std::cout << "   +" << std::string(str.length() + 2, '-') << "+" << std::endl;
    };

    // 6. Fonction d'écho inversée
    auto reverse_echo = [](const std::string& str) {
        std::string reversed(str.rbegin(), str.rend());
        std::cout << "    [REVERSE_ECHO]: " << reversed << std::endl;
    };

    // ─────────────────────────────────────────────────────────────────
    // TESTS : CAS LIMITES ET VALIDATION DE LONGUEUR
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SECTION 1 : TESTS DE VALIDATION DE LONGUEUR                   │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test 1.1 : Chaîne vide (0 octets)
    std::cout << "\n[TEST 1.1] Chaîne vide (0 octets)" << std::endl;
    malicious_echo("", devil_code_basic);

    // Test 1.2 : Chaîne de 1 caractère
    std::cout << "\n[TEST 1.2] Chaîne de 1 caractère (1 octet)" << std::endl;
    malicious_echo("a", devil_code_basic);

    // Test 1.3 : Chaîne de 7 caractères (juste en dessous du seuil)
    std::cout << "\n[TEST 1.3] Chaîne de 7 caractères (limite inférieure)" << std::endl;
    malicious_echo("1234567", devil_code_analyzer);

    // Test 1.4 : Chaîne de 8 caractères (exactement au seuil)
    std::cout << "\n[TEST 1.4] Chaîne de 8 caractères (seuil exact)" << std::endl;
    malicious_echo("12345678", devil_code_basic);

    // Test 1.5 : Chaîne de 9 caractères (juste au-dessus du seuil)
    std::cout << "\n[TEST 1.5] Chaîne de 9 caractères (au-dessus du seuil)" << std::endl;
    malicious_echo("123456789", devil_code_basic);

    // ─────────────────────────────────────────────────────────────────
    // TESTS : DIFFÉRENTS TYPES DE CODE MALICIEUX
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SECTION 2 : TESTS AVEC DIFFÉRENTS CODES MALICIEUX             │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test 2.1 : Code malicieux basique
    std::cout << "\n[TEST 2.1] Devil code basique sur chaîne courte" << std::endl;
    malicious_echo("short", devil_code_basic);

    // Test 2.2 : Code malicieux avec compteur (plusieurs appels)
    std::cout << "\n[TEST 2.2] Devil code avec compteur (3 tentatives)" << std::endl;
    malicious_echo("bad1", devil_code_counter);
    malicious_echo("bad2", devil_code_counter);
    malicious_echo("bad3", devil_code_counter);

    // Test 2.3 : Code malicieux analyzer
    std::cout << "\n[TEST 2.3] Devil code analyzer sur chaîne courte" << std::endl;
    malicious_echo("test", devil_code_analyzer);

    // ─────────────────────────────────────────────────────────────────
    // TESTS : DIFFÉRENTES FONCTIONS D'ÉCHO PERSONNALISÉES
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SECTION 3 : TESTS AVEC DIFFÉRENTES FONCTIONS D'ÉCHO           │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test 3.1 : Écho par défaut (custom_echo)
    std::cout << "\n[TEST 3.1] Écho par défaut (custom_echo)" << std::endl;
    malicious_echo("Valid string for default echo", devil_code_basic);

    // Test 3.2 : Écho en majuscules
    std::cout << "\n[TEST 3.2] Écho en MAJUSCULES" << std::endl;
    malicious_echo("This will be in uppercase", devil_code_basic, upper_case_echo);

    // Test 3.3 : Écho encadré
    std::cout << "\n[TEST 3.3] Écho encadré (boxed)" << std::endl;
    malicious_echo("Beautiful boxed message", devil_code_basic, boxed_echo);

    // Test 3.4 : Écho inversé
    std::cout << "\n[TEST 3.4] Écho inversé (reversed)" << std::endl;
    malicious_echo("Reversed text example", devil_code_basic, reverse_echo);

    // ─────────────────────────────────────────────────────────────────
    // TESTS : CHAÎNES AVEC CARACTÈRES SPÉCIAUX
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SECTION 4 : TESTS AVEC CARACTÈRES SPÉCIAUX                    │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test 4.1 : Chaîne avec espaces
    std::cout << "\n[TEST 4.1] Chaîne avec espaces multiples" << std::endl;
    malicious_echo("  Spaces  everywhere  ", devil_code_basic, boxed_echo);

    // Test 4.2 : Chaîne avec chiffres
    std::cout << "\n[TEST 4.2] Chaîne avec chiffres" << std::endl;
    malicious_echo("Password123456", devil_code_basic, upper_case_echo);

    // Test 4.3 : Chaîne avec caractères spéciaux
    std::cout << "\n[TEST 4.3] Chaîne avec caractères spéciaux" << std::endl;
    malicious_echo("Special!@#$%^&*()", devil_code_analyzer, reverse_echo);

    // Test 4.4 : Chaîne avec accents
    std::cout << "\n[TEST 4.4] Chaîne avec accents" << std::endl;
    malicious_echo("Héllo wörld ñoël", devil_code_basic);

    // ─────────────────────────────────────────────────────────────────
    // TESTS : SCÉNARIOS RÉALISTES
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SECTION 5 : SCÉNARIOS RÉALISTES D'UTILISATION                 │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test 5.1 : Simulation de mot de passe trop court
    std::cout << "\n[TEST 5.1] Simulation : Mot de passe trop court" << std::endl;
    malicious_echo("pass", devil_code_analyzer);

    // Test 5.2 : Simulation de mot de passe valide
    std::cout << "\n[TEST 5.2] Simulation : Mot de passe valide" << std::endl;
    malicious_echo("SecureP@ssw0rd!", devil_code_basic);

    // Test 5.3 : Simulation de nom d'utilisateur invalide
    std::cout << "\n[TEST 5.3] Simulation : Nom d'utilisateur trop court" << std::endl;
    malicious_echo("usr", devil_code_counter);

    // Test 5.4 : Simulation de nom d'utilisateur valide
    std::cout << "\n[TEST 5.4] Simulation : Nom d'utilisateur valide" << std::endl;
    malicious_echo("john_doe_2025", devil_code_basic, upper_case_echo);

    // Test 5.5 : Simulation de message de chat
    std::cout << "\n[TEST 5.5] Simulation : Message de chat" << std::endl;
    malicious_echo("Hello everyone, how are you doing today?", devil_code_basic, boxed_echo);

    // ─────────────────────────────────────────────────────────────────
    // RÉSUMÉ DES TESTS
    // ─────────────────────────────────────────────────────────────────

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RÉSUMÉ DES TESTS                            ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Total d'exécutions de devil_code (avec compteur) : " << malicious_counter << "          ║" << std::endl;
    std::cout << "║ Tous les scénarios ont été testés avec succès                 ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    return 0;
}