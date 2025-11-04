#include "../malicious_echo.cpp"

// Fonction "malicieuse" à exécuter si validation échoue
void devil_function(const std::string& str) {
    std::cout << "MALICIOUS car trop court\n";
}

// Fonction d'écho personnalisée
void custom_echo_function(const std::string& str) {
    std::cout << "ECHO ok\n";
}

int main() {
    
    // Test 1: Chaîne trop courte (< 8 caractères)
    std::cout << "Test 1: Chaîne courte\n";
    malicious_echo("test", devil_function, custom_echo_function);
    
    std::cout << "\n";
    
    // Test 2: Chaîne valide (>= 8 caractères)
    std::cout << "Test 2: Chaîne valide\n";
    malicious_echo("hello world", devil_function, custom_echo_function);
    
    return 0;
}