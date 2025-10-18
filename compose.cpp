#include <iostream>
#include <string>
#include <utility> // Nécessaire pour std::forward

/**
 * @brief Namespace pour encapsuler la fonction de composition.
 * C'est la bonne pratique C++ pour les utilitaires sans état.
 */
namespace ComposeUtils {

    /**
     * @brief Exécute une fonction f un nombre x de fois en chaînant le résultat.
     * * Cette fonction implémente la composition f(f(...f(arg)...)) en utilisant 
     * la récursivité et les templates.
     * Elle suppose que le résultat de f(arg) a le même type que l'argument 'arg'
     * pour que la composition puisse continuer.
     * * @tparam F Type de la fonction (lambda, std::function, pointeur de fonction).
     * @tparam T Type de l'argument initial.
     * @param f La fonction à appliquer.
     * @param x Le nombre d'appels à effectuer.
     * @param arg L'argument initial.
     * @return Le résultat final de la composition.
     */
    template <typename F, typename T>
    auto compose_n_times(F&& f, int x, T&& arg) 
    // Le 'trailing return type' permet de déduire le type de retour 
    // qui est le même que le résultat de f(arg).
    -> decltype(f(std::forward<T>(arg))) 
    {
        // Cas de base : Si x est inférieur ou égal à 0, on s'arrête 
        // et on retourne l'argument tel quel (la fonction identité).
        if (x <= 0) {
            return std::forward<T>(arg); 
        } else {
            // Étape récursive :
            // 1. Appelle la fonction f avec l'argument actuel.
            // 2. Passe le résultat de f à l'appel récursif suivant avec x-1.
            // La récursivité déroule les appels de l'intérieur vers l'extérieur.
            return compose_n_times(
                std::forward<F>(f), 
                x - 1, 
                f(std::forward<T>(arg))
            );
        }
    }
    
    // NOTE TECHNIQUE : Le code ci-dessus implémente la composition 
    // f^x(arg) de manière "naturelle" (sans appel initial séparé).
    
} // namespace ComposeUtils


// -----------------------------------------------------------------
// Fonctions et Structures pour les tests
// -----------------------------------------------------------------

// 1. Fonction simple (Pointeur de fonction)
int multiply_by_two(int value) {
    return value * 2;
}

// 2. Fonction utilisant un type complexe (string)
std::string add_exclamation(const std::string& s) {
    return s + "!";
}

// 3. Structure simple pour tester des objets
struct Point {
    int x, y;
    // Constructeur et méthode pour l'affichage
    Point(int _x, int _y) : x(_x), y(_y) {}
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// 4. Fonction qui modifie la structure Point (doit retourner un Point)
Point move_point_right(Point p) {
    p.x += 1;
    return p; // Retourne le Point modifié
}


int main() {
    using namespace ComposeUtils;
    
    std::cout << "=================================================" << std::endl;
    std::cout << "TESTS DÉTAILLÉS de compose_n_times" << std::endl;
    std::cout << "=================================================" << std::endl;

    // --- TEST 1 : Cas de base avec un pointeur de fonction et entiers ---
    std::cout << "\n--- TEST 1: Multiplication par 2 (Pointeur de fonction) ---" << std::endl;
    
    int initial_value_1 = 5;
    int calls_1 = 3;
    // 5 -> 10 -> 20 -> 40
    int expected_1 = 40; 
    
    int result_1 = compose_n_times(multiply_by_two, calls_1, initial_value_1);
    
    std::cout << "Appel: compose_n_times(mult_by_2, " << calls_1 << ", " << initial_value_1 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_1 << std::endl;
    std::cout << (result_1 == expected_1 ? "Statut: SUCCÈS ✅" : "Statut: ÉCHEC ❌") << std::endl;

    // --- TEST 2 : Cas de base x = 0 (Fonction identité) ---
    std::cout << "\n--- TEST 2: Cas x = 0 ---" << std::endl;
    
    int initial_value_2 = 99;
    int calls_2 = 0;
    
    int result_2 = compose_n_times(multiply_by_two, calls_2, initial_value_2);
    
    std::cout << "Appel: compose_n_times(mult_by_2, " << calls_2 << ", " << initial_value_2 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_2 << std::endl;
    std::cout << (result_2 == initial_value_2 ? "Statut: SUCCÈS ✅" : "Statut: ÉCHEC ❌") << std::endl;
    
    // --- TEST 3 : Utilisation de Lambdas C++11/14 (Opérations plus complexes) ---
    std::cout << "\n--- TEST 3: Utilisation d'une Lambda (Fonction anonyme) ---" << std::endl;

    auto square = [](double x) { return x * x; };
    double initial_value_3 = 2.0;
    int calls_3 = 2;
    // 2.0 -> 4.0 -> 16.0
    double expected_3 = 16.0;
    
    double result_3 = compose_n_times(square, calls_3, initial_value_3);
    
    std::cout << "Appel: compose_n_times(square_lambda, " << calls_3 << ", " << initial_value_3 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_3 << std::endl;
    std::cout << (std::abs(result_3 - expected_3) < 0.001 ? "Statut: SUCCÈS ✅" : "Statut: ÉCHEC ❌") << std::endl;

    // --- TEST 4 : Types non-numériques (std::string) ---
    std::cout << "\n--- TEST 4: Composition avec des std::string ---" << std::endl;
    
    std::string initial_value_4 = "Base";
    int calls_4 = 4;
    // "Base" -> "Base!" -> "Base!!" -> "Base!!!" -> "Base!!!!"
    std::string expected_4 = "Base!!!!";
    
    std::string result_4 = compose_n_times(add_exclamation, calls_4, initial_value_4);
    
    std::cout << "Appel: compose_n_times(add_exclamation, " << calls_4 << ", \"" << initial_value_4 << "\")" << std::endl;
    std::cout << "Résultat obtenu: \"" << result_4 << "\"" << std::endl;
    std::cout << (result_4 == expected_4 ? "Statut: SUCCÈS ✅" : "Statut: ÉCHEC ❌") << std::endl;

    // --- TEST 5 : Utilisation de classes/structs personnalisées ---
    std::cout << "\n--- TEST 5: Composition sur une structure personnalisée (Point) ---" << std::endl;
    
    Point initial_point(0, 5);
    int calls_5 = 10;
    // (0, 5) -> (1, 5) -> ... -> (10, 5)
    Point expected_point(10, 5);
    
    Point result_point = compose_n_times(move_point_right, calls_5, initial_point);
    
    std::cout << "Appel: compose_n_times(move_right, " << calls_5 << ", " << initial_point.toString() << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_point.toString() << std::endl;
    bool success_5 = (result_point.x == expected_point.x && result_point.y == expected_point.y);
    std::cout << (success_5 ? "Statut: SUCCÈS ✅" : "Statut: ÉCHEC ❌") << std::endl;
    
    std::cout << "=================================================" << std::endl;

    return 0;
}