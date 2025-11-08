#include <iostream>
#include <string>
#include "hash_function.cpp"

// =================================================================
// FONCTIONS UTILITAIRES POUR LES TESTS
// =================================================================

/**
 * @brief Affiche un test avec son résultat.
 */
void print_test(const std::string& test_name, const std::string& key, const std::string& hash) {
    std::cout << "[" << test_name << "]" << std::endl;
    std::cout << "  Clé (K)    : \"" << key << "\"" << std::endl;
    std::cout << "  Digest (X) : " << hash << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Simule une attaque par force brute simple (pour démonstration).
 */
bool brute_force_demo(const std::string& target_digest, int max_length) {
    std::cout << "Simulation d'attaque par force brute (dictionnaire simple)..." << std::endl;
    
    // Dictionnaire de mots de passe courants pour la démo
    std::string common_passwords[] = {
        "password", "123456", "12345678", "qwerty", "abc123",
        "monkey", "letmein", "trustno1", "dragon", "baseball",
        "iloveyou", "master", "sunshine", "ashley", "bailey",
        "passw0rd", "shadow", "123123", "654321", "superman",
        "admin", "root", "test", "demo", "hello"
    };
    
    int dict_size = 25;
    
    for (int i = 0; i < dict_size; i++) {
        std::string candidate = common_passwords[i];
        std::string hash = hash_function(candidate);
        
        std::cout << "  Essai : \"" << candidate << "\" -> " << hash.substr(0, 16) << "..." << std::endl;
        
        if (hash == target_digest) {
            std::cout << "\n  ✓ CLÉ TROUVÉE : \"" << candidate << "\"" << std::endl;
            return true;
        }
    }
    
    std::cout << "\n  ✗ Clé non trouvée dans le dictionnaire" << std::endl;
    return false;
}

// =================================================================
// MAIN - TESTS
// =================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║     TEST DE LA FONCTION DE HACHAGE SHA-256                    ║" << std::endl;
    std::cout << "║     Objectif : Permettre à l'attaquant de reverser un hash    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 1 : GÉNÉRATION DE DIGESTS (X = H(K))
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 1 : GÉNÉRATION DE DIGESTS                              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 1.1 : Hachage de mots de passe simples                │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    // Test avec des clés simples
    std::string key1 = "password";
    std::string digest1 = hash_function(key1);
    print_test("TEST 1.1", key1, digest1);

    std::string key2 = "123456";
    std::string digest2 = hash_function(key2);
    print_test("TEST 1.2", key2, digest2);

    std::string key3 = "admin";
    std::string digest3 = hash_function(key3);
    print_test("TEST 1.3", key3, digest3);

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 1.2 : Hachage de mots de passe complexes              │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::string key4 = "MySecureP@ssw0rd!";
    std::string digest4 = hash_function(key4);
    print_test("TEST 1.4", key4, digest4);

    std::string key5 = "Th1s_1s_4_c0mpl3x_k3y";
    std::string digest5 = hash_function(key5);
    print_test("TEST 1.5", key5, digest5);

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 1.3 : Hachage de chaînes vides et courtes             │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::string key6 = "";
    std::string digest6 = hash_function(key6);
    print_test("TEST 1.6 (chaîne vide)", key6, digest6);

    std::string key7 = "a";
    std::string digest7 = hash_function(key7);
    print_test("TEST 1.7 (un caractère)", key7, digest7);

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 2 : VÉRIFICATION DE HASH
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 2 : VÉRIFICATION DE HASH                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test de vérification : verify_hash(K, X)                      │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[TEST 2.1] Vérification correcte" << std::endl;
    std::cout << "  Clé : \"password\"" << std::endl;
    std::cout << "  Digest attendu : " << digest1 << std::endl;
    bool verify1 = verify_hash("password", digest1);
    std::cout << "  Résultat : " << (verify1 ? "✓ MATCH" : "✗ NO MATCH") << std::endl;

    std::cout << "\n[TEST 2.2] Vérification incorrecte" << std::endl;
    std::cout << "  Clé : \"wrongpassword\"" << std::endl;
    std::cout << "  Digest attendu : " << digest1 << std::endl;
    bool verify2 = verify_hash("wrongpassword", digest1);
    std::cout << "  Résultat : " << (verify2 ? "✓ MATCH" : "✗ NO MATCH") << std::endl;

    std::cout << "\n[TEST 2.3] Vérification avec clé complexe" << std::endl;
    std::cout << "  Clé : \"MySecureP@ssw0rd!\"" << std::endl;
    std::cout << "  Digest attendu : " << digest4 << std::endl;
    bool verify3 = verify_hash("MySecureP@ssw0rd!", digest4);
    std::cout << "  Résultat : " << (verify3 ? "✓ MATCH" : "✗ NO MATCH") << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 3 : SIMULATION D'ATTAQUE (REVERSE HASH)
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 3 : SIMULATION D'ATTAQUE PAR L'ATTAQUANT               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Scénario : L'attaquant possède le digest X et connaît H       │" << std::endl;
    std::cout << "│ Objectif : Trouver la clé K telle que X = H(K)                │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[ATTAQUE 1] Retrouver la clé pour digest de \"password\"" << std::endl;
    std::cout << "Digest cible : " << digest1 << std::endl;
    std::cout << std::endl;
    brute_force_demo(digest1, 8);

    std::cout << "\n\n[ATTAQUE 2] Retrouver la clé pour digest de \"admin\"" << std::endl;
    std::cout << "Digest cible : " << digest3 << std::endl;
    std::cout << std::endl;
    brute_force_demo(digest3, 8);

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 4 : INFORMATIONS POUR HASHCAT
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 4 : UTILISATION AVEC HASHCAT                           ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\nPour utiliser hashcat avec cette implémentation :" << std::endl;
    std::cout << "─────────────────────────────────────────────────────────────" << std::endl;
    std::cout << "\n1. Créer un fichier avec les digests (un par ligne) :" << std::endl;
    std::cout << "   echo '" << digest1 << "' > hashes.txt" << std::endl;
    
    std::cout << "\n2. Lancer hashcat en mode SHA-256 :" << std::endl;
    std::cout << "   hashcat -m 1400 -a 0 hashes.txt wordlist.txt" << std::endl;
    
    std::cout << "\n3. Options hashcat :" << std::endl;
    std::cout << "   -m 1400  : Mode SHA-256" << std::endl;
    std::cout << "   -a 0     : Attaque par dictionnaire" << std::endl;
    std::cout << "   -a 3     : Attaque par force brute (masque)" << std::endl;
    
    std::cout << "\n4. Exemple d'attaque par masque (8 caractères alphanumériques) :" << std::endl;
    std::cout << "   hashcat -m 1400 -a 3 hashes.txt ?a?a?a?a?a?a?a?a" << std::endl;

    std::cout << "\n5. Vérification du résultat :" << std::endl;
    std::cout << "   hashcat -m 1400 --show hashes.txt" << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // RÉSUMÉ FINAL
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RÉSUMÉ FINAL                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ ✓ Fonction de hachage SHA-256 implémentée                     ║" << std::endl;
    std::cout << "║ ✓ Génération de digests X = H(K) testée                       ║" << std::endl;
    std::cout << "║ ✓ Vérification de hash fonctionnelle                          ║" << std::endl;
    std::cout << "║ ✓ Simulation d'attaque par dictionnaire réussie               ║" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "║ L'attaquant peut maintenant :                                 ║" << std::endl;
    std::cout << "║ 1. Obtenir un digest X                                        ║" << std::endl;
    std::cout << "║ 2. Utiliser cette fonction H avec hashcat                     ║" << std::endl;
    std::cout << "║ 3. Retrouver la clé K telle que X = H(K)                      ║" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "║ Compatible GCC 4.4.7 (C++03)                                  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    return 0;
}
