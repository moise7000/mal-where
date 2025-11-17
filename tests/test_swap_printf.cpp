#include <iostream>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include "TestingTools.h"
// Forward declarations for swap functions
extern void init_printf_swap();
extern int swapped_printf(const char* format, ...);
extern void swapped_OutputDebugStringA(const char* message);
#else
#include <syslog.h>
#endif

// =================================================================
// TEST SWAP PRINTF - Windows 7+ et Linux
// =================================================================

#ifndef _WIN32
// =================================================================
// TESTS LINUX - printf et syslog normaux
// =================================================================

/**
 * @brief Test Linux 1: Test printf standard
 */
void test_linux_printf() {
    std::cout << "\n=== Test printf standard (Linux) ===" << std::endl;
    std::cout << "NOTE: Affichage direct sur stdout" << std::endl;
    
    std::cout << "Appel de printf(\"Test printf Linux: Hello!\\n\")..." << std::endl;
    int result = printf("Test printf Linux: Hello!\n");
    
    if (result > 0) {
        std::cout << "[OK] printf a retourne " << result << " caracteres" << std::endl;
    } else {
        std::cout << "[FAIL] printf a echoue" << std::endl;
    }
}

/**
 * @brief Test Linux 2: Test syslog standard
 */
void test_linux_syslog() {
    std::cout << "\n=== Test syslog standard (Linux) ===" << std::endl;
    std::cout << "NOTE: Message envoye vers syslog" << std::endl;
    std::cout << "      Verifier avec: sudo tail -f /var/log/syslog" << std::endl;
    std::cout << "      ou: journalctl -f" << std::endl;
    
    openlog("test_swap_printf", LOG_PID | LOG_CONS, LOG_USER);
    std::cout << "Appel de syslog(LOG_INFO, \"Test syslog Linux: Hello!\")..." << std::endl;
    syslog(LOG_INFO, "Test syslog Linux: Hello from test_swap_printf!");
    closelog();
    
    std::cout << "[OK] syslog execute (journalctl --since \"1 minute ago\" | grep test_swap_printf)" << std::endl;
}

/**
 * @brief Test Linux 3: Format complexe avec printf
 */
void test_linux_printf_complex() {
    std::cout << "\n=== Test format complexe printf (Linux) ===" << std::endl;
    
    int value = 42;
    const char* str = "test";
    float pi = 3.14159f;
    
    std::cout << "Appel avec format complexe..." << std::endl;
    int result = printf("Int=%d, Str=%s, Float=%.2f\n", value, str, pi);
    
    if (result > 0) {
        std::cout << "[OK] Format complexe traite correctement (" << result << " chars)" << std::endl;
    } else {
        std::cout << "[FAIL] Echec du format complexe" << std::endl;
    }
}

/**
 * @brief Test Linux 4: Format complexe avec syslog
 */
void test_linux_syslog_complex() {
    std::cout << "\n=== Test format complexe syslog (Linux) ===" << std::endl;
    std::cout << "NOTE: Verifier dans les logs systeme" << std::endl;
    
    int value = 123;
    const char* str = "malware";
    
    openlog("test_swap_printf", LOG_PID, LOG_USER);
    std::cout << "Appel de syslog avec format complexe..." << std::endl;
    syslog(LOG_WARNING, "Test complexe: value=%d, string=%s", value, str);
    closelog();
    
    std::cout << "[OK] syslog avec format execute (sudo tail -20 /var/log/syslog | grep test_swap_printf)" << std::endl;
}

/**
 * @brief Test Linux 5: Note sur LD_PRELOAD
 */
void test_linux_ld_preload_info() {
    std::cout << "\n=== Information LD_PRELOAD (Linux) ===" << std::endl;
    std::cout << "Pour tester le SWAP printf <-> syslog sur Linux:" << std::endl;
    std::cout << "1. Compiler la bibliotheque swap:" << std::endl;
    std::cout << "   g++ -shared -fPIC -o libswap_printf.so obfuscation_methods/swap_printf.cpp" << std::endl;
    std::cout << "2. Executer avec LD_PRELOAD:" << std::endl;
    std::cout << "   LD_PRELOAD=./libswap_printf.so ./swap_printf.exe" << std::endl;
    std::cout << "3. Resultat attendu:" << std::endl;
    std::cout << "   - printf() enverra vers syslog (invisible dans terminal)" << std::endl;
    std::cout << "   - syslog() utilisera printf (visible dans terminal)" << std::endl;
    std::cout << "[INFO] Test informatif complete" << std::endl;
}

#else
// =================================================================
// TESTS WINDOWS - swap explicite
// =================================================================

/**
 * @brief Test 1: Vérifier que swapped_printf fonctionne
 * Au lieu d'afficher sur stdout, devrait envoyer vers OutputDebugStringA
 */
void test_swapped_printf() {
    std::cout << "\n=== Test swapped_printf ===" << std::endl;
    std::cout << "NOTE: swapped_printf devrait envoyer vers OutputDebugStringA" << std::endl;
    std::cout << "      (visible dans DebugView ou debugger)" << std::endl;
    
#ifdef _WIN32
    init_printf_swap();
    
    // Appel de swapped_printf - devrait aller vers OutputDebugStringA
    std::cout << "Appel de swapped_printf(\"Hello from swapped printf!\\n\")..." << std::endl;
    int result = swapped_printf("Hello from swapped printf!\n");
    
    if (result > 0) {
        std::cout << "[OK] swapped_printf a retourne " << result << " caracteres" << std::endl;
    } else {
        std::cout << "[FAIL] swapped_printf a echoue" << std::endl;
    }
#else
    std::cout << "[SKIP] Test disponible uniquement sur Windows" << std::endl;
#endif
}

/**
 * @brief Test 2: Vérifier que swapped_OutputDebugStringA fonctionne
 * Au lieu d'envoyer vers le debugger, devrait utiliser printf
 */
void test_swapped_OutputDebugStringA() {
    std::cout << "\n=== Test swapped_OutputDebugStringA ===" << std::endl;
    std::cout << "NOTE: swapped_OutputDebugStringA devrait utiliser printf" << std::endl;
    std::cout << "      (visible directement dans le terminal)" << std::endl;
    
#ifdef _WIN32
    init_printf_swap();
    
    // Appel de swapped_OutputDebugStringA - devrait utiliser printf
    std::cout << "Appel de swapped_OutputDebugStringA(\"Hello from swapped OutputDebugString!\\n\")..." << std::endl;
    std::cout << "Output attendu ci-dessous:" << std::endl;
    swapped_OutputDebugStringA("Hello from swapped OutputDebugString!\n");
    std::cout << "[OK] swapped_OutputDebugStringA execute (voir output ci-dessus)" << std::endl;
#else
    std::cout << "[SKIP] Test disponible uniquement sur Windows" << std::endl;
#endif
}

/**
 * @brief Test 3: Vérifier l'initialisation
 */
void test_initialization() {
    std::cout << "\n=== Test initialization ===" << std::endl;
    
#ifdef _WIN32
    init_printf_swap();
    std::cout << "[OK] init_printf_swap() execute sans erreur" << std::endl;
    
    // Double init ne devrait pas poser de problème
    init_printf_swap();
    std::cout << "[OK] Double initialisation OK" << std::endl;
#else
    std::cout << "[SKIP] Test disponible uniquement sur Windows" << std::endl;
#endif
}

/**
 * @brief Test 4: Test avec format complexe
 */
void test_complex_format() {
    std::cout << "\n=== Test format complexe ===" << std::endl;
    
#ifdef _WIN32
    init_printf_swap();
    
    int value = 42;
    const char* str = "test";
    float pi = 3.14159f;
    
    std::cout << "Appel avec format complexe: swapped_printf(\"Int=%d, Str=%s, Float=%.2f\\n\", 42, \"test\", 3.14159f)" << std::endl;
    int result = swapped_printf("Int=%d, Str=%s, Float=%.2f\n", value, str, pi);
    
    if (result > 0) {
        std::cout << "[OK] Format complexe traite correctement (" << result << " chars)" << std::endl;
    } else {
        std::cout << "[FAIL] Echec du format complexe" << std::endl;
    }
#else
    std::cout << "[SKIP] Test disponible uniquement sur Windows" << std::endl;
#endif
}

#endif // _WIN32

// =================================================================
// MAIN - EXÉCUTION DES TESTS
// =================================================================

int main() {
    std::cout << "============================================" << std::endl;
#ifdef _WIN32
    std::cout << "   TESTS SWAP PRINTF (Windows 7+)          " << std::endl;
#else
    std::cout << "   TESTS SWAP PRINTF (Linux)               " << std::endl;
#endif
    std::cout << "============================================" << std::endl;
    
#ifdef _WIN32
    // Tests Windows
    test_initialization();
    test_swapped_printf();
    test_swapped_OutputDebugStringA();
    test_complex_format();
#else
    // Tests Linux
    std::cout << "\n[INFO] Plateforme Linux detectee" << std::endl;
    std::cout << "       Tests des fonctions standard printf/syslog" << std::endl;
    
    test_linux_printf();
    test_linux_syslog();
    test_linux_printf_complex();
    test_linux_syslog_complex();
    test_linux_ld_preload_info();
#endif
    
    std::cout << "\n============================================" << std::endl;
    std::cout << "       TESTS SWAP PRINTF TERMINES          " << std::endl;
    std::cout << "============================================" << std::endl;
    
    return 0;
}
