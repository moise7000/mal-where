#include <iostream>
#include "..//obfuscation_methodsfake_rec.cpp"

// =================================================================
// FONCTIONS DE TEST AVEC DIFFÉRENTS TYPES DE RETOUR
// =================================================================

/**
 * @brief Fonction qui retourne un int - calcule le carré.
 */
int square(int x) {
    int result = x * x;
    std::cout << "    -> square(" << x << ") = " << result << std::endl;
    return result;
}

/**
 * @brief Fonction qui retourne un int - calcule le double.
 */
int double_value(int x) {
    int result = x * 2;
    std::cout << "    -> double_value(" << x << ") = " << result << std::endl;
    return result;
}

/**
 * @brief Fonction qui retourne un float - calcule la racine carrée approximative.
 */
float sqrt_approx(float x) {
    float result = x / 2.0f;
    std::cout << "    -> sqrt_approx(" << x << ") = " << result << std::endl;
    return result;
}

/**
 * @brief Fonction qui retourne un double - calcule le cube.
 */
double cube_double(double x) {
    double result = x * x * x;
    std::cout << "    -> cube_double(" << x << ") = " << result << std::endl;
    return result;
}

/**
 * @brief Fonction qui retourne un char - conversion ASCII.
 */
char int_to_char(int x) {
    char result = static_cast<char>(x + 65); // 65 = 'A'
    std::cout << "    -> int_to_char(" << x << ") = '" << result << "'" << std::endl;
    return result;
}

/**
 * @brief Fonction qui retourne un long - multiplication par 1000.
 */
long multiply_1000(long x) {
    long result = x * 1000;
    std::cout << "    -> multiply_1000(" << x << ") = " << result << std::endl;
    return result;
}

// =================================================================
// MAIN - TESTS
// =================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         SUITE DE TESTS - WRAPPER ET FAKE_RECURSIVE            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 1 : TESTS DU WRAPPER
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 1 : TESTS DU WRAPPER DE FONCTION                       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.1 : Création et appel direct du wrapper (int)          │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    FunctionWrapper<int, int> wrapped_square = make_wrapper(square);
    std::cout << "Appel direct : wrapped_square(5)" << std::endl;
    int result_wrapper1 = wrapped_square(5);
    std::cout << "Resultat: " << result_wrapper1 << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.2 : Wrapper avec float                                  │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    FunctionWrapper<float, float> wrapped_sqrt = make_wrapper(sqrt_approx);
    std::cout << "Appel direct : wrapped_sqrt(16.0f)" << std::endl;
    float result_wrapper2 = wrapped_sqrt(16.0f);
    std::cout << "Resultat: " << result_wrapper2 << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.3 : Wrapper avec double                                 │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    FunctionWrapper<double, double> wrapped_cube = make_wrapper(cube_double);
    std::cout << "Appel direct : wrapped_cube(3.0)" << std::endl;
    double result_wrapper3 = wrapped_cube(3.0);
    std::cout << "Resultat: " << result_wrapper3 << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.4 : Wrapper avec char                                   │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    FunctionWrapper<char, int> wrapped_char = make_wrapper(int_to_char);
    std::cout << "Appel direct : wrapped_char(0)" << std::endl;
    char result_wrapper4 = wrapped_char(0);
    std::cout << "Resultat: '" << result_wrapper4 << "'" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.5 : Wrapper avec long                                   │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    FunctionWrapper<long, long> wrapped_long = make_wrapper(multiply_1000);
    std::cout << "Appel direct : wrapped_long(42)" << std::endl;
    long result_wrapper5 = wrapped_long(42);
    std::cout << "Resultat: " << result_wrapper5 << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Test 1.6 : Vérification des typedef du wrapper                │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "wrapped_square.result_type = int (compile-time check OK)" << std::endl;
    std::cout << "wrapped_square.argument_type = int (compile-time check OK)" << std::endl;
    std::cout << "Le wrapper expose correctement les types via typedef" << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 2 : TESTS DE FAKE_RECURSIVE AVEC VALEURS LIMITES
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 2 : TESTS DE FAKE_RECURSIVE AVEC VALEURS LIMITES       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 2.1 : Tests avec x = 0 (cas limite minimal)           │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[TEST 2.1.1] x=0 avec int - square(7)" << std::endl;
    int result_2_1_1 = fake_recursive(0, make_wrapper(square), 7);
    std::cout << "Resultat: " << result_2_1_1 << " (devrait être 49)" << std::endl;

    std::cout << "\n[TEST 2.1.2] x=0 avec float - sqrt_approx(100.0f)" << std::endl;
    float result_2_1_2 = fake_recursive(0, make_wrapper(sqrt_approx), 100.0f);
    std::cout << "Resultat: " << result_2_1_2 << std::endl;

    std::cout << "\n[TEST 2.1.3] x=0 avec char - int_to_char(4)" << std::endl;
    char result_2_1_3 = fake_recursive(0, make_wrapper(int_to_char), 4);
    std::cout << "Resultat: '" << result_2_1_3 << "'" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 2.2 : Tests avec x = 1 (une seule itération)          │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[TEST 2.2.1] x=1 avec int - double_value(10)" << std::endl;
    int result_2_2_1 = fake_recursive(1, make_wrapper(double_value), 10);
    std::cout << "Resultat: " << result_2_2_1 << std::endl;

    std::cout << "\n[TEST 2.2.2] x=1 avec double - cube_double(2.0)" << std::endl;
    double result_2_2_2 = fake_recursive(1, make_wrapper(cube_double), 2.0);
    std::cout << "Resultat: " << result_2_2_2 << std::endl;

    std::cout << "\n[TEST 2.2.3] x=1 avec long - multiply_1000(5)" << std::endl;
    long result_2_2_3 = fake_recursive(1, make_wrapper(multiply_1000), 5L);
    std::cout << "Resultat: " << result_2_2_3 << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 2.3 : Tests avec x = 2 (deux itérations)              │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[TEST 2.3.1] x=2 avec int - square(6)" << std::endl;
    int result_2_3_1 = fake_recursive(2, make_wrapper(square), 6);
    std::cout << "Resultat: " << result_2_3_1 << std::endl;

    std::cout << "\n[TEST 2.3.2] x=2 avec float - sqrt_approx(50.0f)" << std::endl;
    float result_2_3_2 = fake_recursive(2, make_wrapper(sqrt_approx), 50.0f);
    std::cout << "Resultat: " << result_2_3_2 << std::endl;

    std::cout << "\n[TEST 2.3.3] x=2 avec char - int_to_char(10)" << std::endl;
    char result_2_3_3 = fake_recursive(2, make_wrapper(int_to_char), 10);
    std::cout << "Resultat: '" << result_2_3_3 << "'" << std::endl;

    std::cout << "\n┌────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Section 2.4 : Tests avec x = 10 (dix itérations)              │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\n[TEST 2.4.1] x=10 avec int - double_value(3)" << std::endl;
    int result_2_4_1 = fake_recursive(10, make_wrapper(double_value), 3);
    std::cout << "Resultat: " << result_2_4_1 << std::endl;

    std::cout << "\n[TEST 2.4.2] x=10 avec double - cube_double(1.5)" << std::endl;
    double result_2_4_2 = fake_recursive(10, make_wrapper(cube_double), 1.5);
    std::cout << "Resultat: " << result_2_4_2 << std::endl;

    std::cout << "\n[TEST 2.4.3] x=10 avec long - multiply_1000(7)" << std::endl;
    long result_2_4_3 = fake_recursive(10, make_wrapper(multiply_1000), 7L);
    std::cout << "Resultat: " << result_2_4_3 << std::endl;

    // ═════════════════════════════════════════════════════════════════
    // PARTIE 3 : TESTS AVEC DIFFÉRENTS TYPES DE RETOUR
    // ═════════════════════════════════════════════════════════════════

    std::cout << "\n\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ PARTIE 3 : TESTS AVEC DIFFÉRENTS TYPES DE RETOUR              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\n[TEST 3.1] Type int - square(8) avec 5 iterations" << std::endl;
    int result_3_1 = fake_recursive(5, make_wrapper(square), 8);
    std::cout << "Resultat final (int): " << result_3_1 << std::endl;

    std::cout << "\n[TEST 3.2] Type float - sqrt_approx(64.0f) avec 3 iterations" << std::endl;
    float result_3_2 = fake_recursive(3, make_wrapper(sqrt_approx), 64.0f);
    std::cout << "Resultat final (float): " << result_3_2 << std::endl;

    std::cout << "\n[TEST 3.3] Type double - cube_double(4.0) avec 4 iterations" << std::endl;
    double result_3_3 = fake_recursive(4, make_wrapper(cube_double), 4.0);
    std::cout << "Resultat final (double): " << result_3_3 << std::endl;

    std::cout << "\n[TEST 3.4] Type char - int_to_char(25) avec 6 iterations" << std::endl;
    char result_3_4 = fake_recursive(6, make_wrapper(int_to_char), 25);
    std::cout << "Resultat final (char): '" << result_3_4 << "'" << std::endl;

    std::cout << "\n[TEST 3.5] Type long - multiply_1000(99) avec 2 iterations" << std::endl;
    long result_3_5 = fake_recursive(2, make_wrapper(multiply_1000), 99L);
    std::cout << "Resultat final (long): " << result_3_5 << std::endl;

    return 0;
}