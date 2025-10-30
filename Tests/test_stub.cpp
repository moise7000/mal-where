//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

#include "../packer/Stub.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <windows.h> // Pour changer la couleur du texte

// Fonctions utilitaires pour colorer le texte
void printGreen(const std::string& msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10); // vert
    std::cout << msg << std::endl;
    SetConsoleTextAttribute(hConsole, 7);  // couleur par défaut
}

void printRed(const std::string& msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12); // rouge
    std::cout << msg << std::endl;
    SetConsoleTextAttribute(hConsole, 7);  // couleur par défaut
}

void printYellow(const std::string& msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 14); // jaune
    std::cout << msg << std::endl;
    SetConsoleTextAttribute(hConsole, 7);  // couleur par défaut
}

// Fonction utilitaire pour afficher un vecteur de bytes
void printBytes(const std::vector<unsigned char>& data) {
    std::cout << "[ ";
    for (size_t i = 0; i < data.size() && i < 20; ++i) {
        std::cout << static_cast<int>(data[i]) << " ";
    }
    if (data.size() > 20) {
        std::cout << "... (" << data.size() << " bytes total)";
    }
    std::cout << " ]" << std::endl;
}

// Test 1 : Construction et destruction basique
void testConstructorBasic() {
    std::cout << "\n=== Test 1: Constructeur basique ===" << std::endl;

    try {
        Packer packer("ma_cle_secrete");
        printGreen(" Constructeur basique OK");
    } catch (const std::exception& e) {
        printRed(std::string(" Erreur: ") + e.what());
    }
}

// Test 2 : Construction avec encodage
void testConstructorWithEncoding() {
    std::cout << "\n=== Test 2: Constructeur avec encodage ===" << std::endl;

    try {
        Stub StubPlain("ma_cle", KeyEncoding::PLAIN);
        printGreen("Constructeur PLAIN OK");

        Stub StubB64("bWFfY2xl", KeyEncoding::BASE64);
        printGreen("Constructeur BASE64 OK");

        Stub StubB32("NV4SA3TPOI======", KeyEncoding::BASE32);
        printGreen("Constructeur BASE32 OK");
    } catch (const std::exception& e) {
        printRed(std::string(" Erreur: ") + e.what());
    }
}

// Test 3 : Déchiffrement et décompression simple
void testUnpackSimple() {
    std::cout << "\n=== Test 3: Déchiffrement et décompression (unpack) ===" << std::endl;

    try {
        Stub Stub("MaSuperCleSecrete123");

        unsigned char packed_array[] = {
            0x06, 0x82, 0x87, 0x8f, 0x82, 0x90, 0x19, 0xbc,
            0x98, 0xe8, 0x83, 0x34, 0x0d, 0xba, 0x35, 0x81
        };

        std::vector<unsigned char> packed(packed_array, packed_array + sizeof(packed_array));
        size_t originalSize = sizeof(packed_array);

        std::cout << "Message chiffré/compressé: ";
        printBytes(packed);

        std::vector<unsigned char> result = Stub.unpack(packed, originalSize);

        std::cout << "Message déchiffré/décompressé: ";
        printBytes(result);
        std::cout << "Taille: " << result.size() << " bytes" << std::endl;
        printGreen(" Test unpack OK");

    } catch (const std::exception& e) {
        printRed(std::string("✗ Erreur: ") + e.what());
    }
}

// Fonction principale de test
int main() {
    printYellow("========================================");
    printYellow("   TESTS DE LA CLASSE Stub");
    printYellow("========================================");

    testConstructorBasic();
    testConstructorWithEncoding();
    testUnpackSimple();

    printYellow("\n========================================");
    printYellow("   FIN DES TESTS");
    printYellow("========================================\n");

    return 0;
}
