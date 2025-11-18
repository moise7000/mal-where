// Test devil code - zip bombs + malicious echo
// Compatible GCC 4.4.7
//
#include <iostream>
#include <string>
#include <fstream>
#include "../devil/DevilCode.h"
#include "../devil/zipbomb.h"
#include "../devil/MaliciousEcho.h"


void normal_echo(const std::string& s) {
    std::cout << "[ECHO] " << s << std::endl;
}

// Wrapper pour MaliciousEcho (qui attend une fonction avec string en param)
void devil_wrapper(const std::string& s) {
    devil();
}

void print_zipbomb_stats(const char* filename, unsigned long size_mb) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        in.seekg(0, std::ios::end);
        unsigned long file_size = in.tellg();
        in.close();
        
        unsigned long orig_size = size_mb * 1024 * 1024;
        unsigned long ratio = orig_size / file_size;
        std::cout << "  -> " << filename << ": " << file_size << " bytes (ratio ~" << ratio << "x)" << std::endl;
    }
}

int main() {

    
    // Test 1: Zip bombs creation only (safe)
    std::cout << "[TEST 1] Creation de zip bombs (safe)" << std::endl;
    
    create_zipbomb("test_zipbomb_50mb_5levels.bin", 50, 5);
    print_zipbomb_stats("test_zipbomb_50mb_5levels.bin", 50);
    
    create_zipbomb("test_zipbomb_100mb_6levels.bin", 100, 6);
    print_zipbomb_stats("test_zipbomb_100mb_6levels.bin", 100);
    
    create_zipbomb("test_zipbomb_500mb_7levels.bin", 500, 7);
    print_zipbomb_stats("test_zipbomb_500mb_7levels.bin", 500);
    
    create_zipbomb("test_zipbomb_1tb_10levels.bin", 1024 * 1024, 10);
    print_zipbomb_stats("test_zipbomb_1tb_10levels.bin", 1024 * 1024);
    
    std::cout << "[DONE] 4 zip bombs creees (derniere: 1 To!)" << std::endl;
    
    // Test 2: Malicious echo
    std::cout << "\n[TEST 2] Malicious echo" << std::endl;
    std::cout << "Chaine valide (>=8 chars) -> echo normal" << std::endl;
    malicious_echo("valid_string", devil_wrapper, normal_echo);
    
    // Test 3: DEVIL CODE (DANGEREUX)
    std::cout << "\n[TEST 3] Devil code (DANGEREUX)" << std::endl;
    std::cout << "\n========================================" << std::endl;
    std::cout << "Le devil code va:" << std::endl;
    std::cout << "  1. Lancer un rickroll" << std::endl;
    std::cout << "  2. Creer une zip bomb (1 TO = 1024 GB)" << std::endl;
    std::cout << "  3. DETONER la zip bomb" << std::endl;
    std::cout << "\n VM avec snapshot obligatoire !" << std::endl;
    std::cout << "\nTaper 'DEVIL' pour continuer (autre chose = annuler): ";
    
    std::string confirmation;
    std::cin >> confirmation;
    
    if (confirmation == "DEVIL") {
        std::cout << "\n[WARNING] Lancement du devil code..." << std::endl;
        std::cout << "[DEVIL] Etape 1/3: Rickroll..." << std::endl;
        std::cout << "[DEVIL] Etape 2/3: Creation zip bomb (1 TO = 1024 GB, 10 niveaux)..." << std::endl;
        std::cout << "[DEVIL] Etape 3/3: DETONATION..." << std::endl;
        
        //devil();
        
        std::cout << "[DEVIL] Terminé (si vous lisez ça, vous avez survécu!)" << std::endl;
    } else {
        std::cout << "\n[INFO] Devil code annulé" << std::endl;
    }
    
    std::cout << "\n[DONE] Tests terminés" << std::endl;
    std::cout << "Fichiers crees:" << std::endl;
    std::cout << "  - test_zipbomb_50mb_5levels.bin" << std::endl;
    std::cout << "  - test_zipbomb_100mb_6levels.bin" << std::endl;
    std::cout << "  - test_zipbomb_500mb_7levels.bin" << std::endl;
    std::cout << "  - test_zipbomb_1tb_10levels.bin (1 TERAOCTET!)" << std::endl;
    
    return 0;
}