#include "../obfuscation_methods/compose.cpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

// 1. Fonction simple (Pointeur de fonction)
int multiply_by_two(int value) {
    return value * 2;
}

// 2. Fonction utilisant un type complexe (string)
std::string add_exclamation(const std::string& s) {
    return s + "!";
}

// 3. Fonction pour doubler un nombre à virgule
double square(double x) {
    return x * x;
}

// 4. Structure simple pour tester des objets
struct Point {
    int x, y;
    
    // Constructeur
    Point(int _x, int _y) : x(_x), y(_y) {}
    
    // Méthode pour l'affichage (compatible C++03)
    std::string toString() const {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ")";
        return oss.str();
    }
};

// 5. Fonction qui modifie la structure Point (doit retourner un Point)
Point move_point_right(Point p) {
    p.x += 1;
    return p; // Retourne le Point modifié
}

// 6. Foncteur pour addition
struct Adder {
    int value_to_add;
    
    Adder(int val) : value_to_add(val) {}
    
    int operator()(int x) const {
        return x + value_to_add;
    }
};


int main() {
    // --- TEST 1 : Cas de base avec un pointeur de fonction et entiers ---
    std::cout << "\n--- TEST 1: Multiplication par 2 (Pointeur de fonction) ---" << std::endl;
    
    int initial_value_1 = 5;
    int calls_1 = 3;
    // 5 -> 10 -> 20 -> 40
    int expected_1 = 40; 
    
    int result_1 = compose_n_times(multiply_by_two, calls_1, initial_value_1);
    
    std::cout << "Appel: compose_n_times(mult_by_2, " << calls_1 << ", " << initial_value_1 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_1 << std::endl;
    std::cout << (result_1 == expected_1 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;

    // --- TEST 2 : Cas de base x = 0 (Fonction identité) ---
    std::cout << "\n--- TEST 2: Cas x = 0 ---" << std::endl;
    
    int initial_value_2 = 99;
    int calls_2 = 0;
    
    int result_2 = compose_n_times(multiply_by_two, calls_2, initial_value_2);
    
    std::cout << "Appel: compose_n_times(mult_by_2, " << calls_2 << ", " << initial_value_2 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_2 << std::endl;
    std::cout << (result_2 == initial_value_2 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;
    
    // --- TEST 3 : Utilisation d'une fonction avec doubles ---
    std::cout << "\n--- TEST 3: Fonction square (double) ---" << std::endl;

    double initial_value_3 = 2.0;
    int calls_3 = 2;
    // 2.0 -> 4.0 -> 16.0
    double expected_3 = 16.0;
    
    double result_3 = compose_n_times(square, calls_3, initial_value_3);
    
    std::cout << "Appel: compose_n_times(square, " << calls_3 << ", " << initial_value_3 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_3 << std::endl;
    std::cout << (std::abs(result_3 - expected_3) < 0.001 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;

    // --- TEST 4 : Types non-numériques (std::string) ---
    std::cout << "\n--- TEST 4: Composition avec des std::string ---" << std::endl;
    
    std::string initial_value_4 = "Base";
    int calls_4 = 4;
    // "Base" -> "Base!" -> "Base!!" -> "Base!!!" -> "Base!!!!"
    std::string expected_4 = "Base!!!!";
    
    std::string result_4 = compose_n_times(add_exclamation, calls_4, initial_value_4);
    
    std::cout << "Appel: compose_n_times(add_exclamation, " << calls_4 << ", \"" << initial_value_4 << "\")" << std::endl;
    std::cout << "Résultat obtenu: \"" << result_4 << "\"" << std::endl;
    std::cout << (result_4 == expected_4 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;

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
    std::cout << (success_5 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;

    // --- TEST 6 : Utilisation d'un foncteur (struct avec operator()) ---
    std::cout << "\n--- TEST 6: Foncteur Adder ---" << std::endl;
    
    Adder add_five(5);
    int initial_value_6 = 0;
    int calls_6 = 4;
    // 0 -> 5 -> 10 -> 15 -> 20
    int expected_6 = 20;
    
    int result_6 = compose_n_times(add_five, calls_6, initial_value_6);
    
    std::cout << "Appel: compose_n_times(add_five, " << calls_6 << ", " << initial_value_6 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_6 << std::endl;
    std::cout << (result_6 == expected_6 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;
    
    // --- TEST 7 : Cas avec x négatif ---
    std::cout << "\n--- TEST 7: Cas x négatif (doit retourner arg inchangé) ---" << std::endl;
    
    int initial_value_7 = 42;
    int calls_7 = -5;
    
    int result_7 = compose_n_times(multiply_by_two, calls_7, initial_value_7);
    
    std::cout << "Appel: compose_n_times(mult_by_2, " << calls_7 << ", " << initial_value_7 << ")" << std::endl;
    std::cout << "Résultat obtenu: " << result_7 << std::endl;
    std::cout << (result_7 == initial_value_7 ? "Statut: SUCCÈS" : "Statut: ÉCHEC") << std::endl;

    return 0;
}
