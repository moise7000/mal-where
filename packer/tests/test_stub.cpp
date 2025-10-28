//
// Fichier de test pour la classe Packer
// Created on 10/28/25
//

#include "../Stub.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

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
        std::cout << "✓ Constructeur basique OK" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 2 : Construction avec encodage
void testConstructorWithEncoding() {
    std::cout << "\n=== Test 2: Constructeur avec encodage ===" << std::endl;

    try {
        Packer packerPlain("ma_cle", KeyEncoding::PLAIN);
        std::cout << "✓ Constructeur PLAIN OK" << std::endl;

        Packer packerB64("bWFfY2xl", KeyEncoding::BASE64);
        std::cout << "✓ Constructeur BASE64 OK" << std::endl;

        Packer packerB32("NV4SA3TPOI======", KeyEncoding::BASE32);
        std::cout << "✓ Constructeur BASE32 OK" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 3 : Déchiffrement et décompression simple
void testUnpackSimple() {
    std::cout << "\n=== Test 3: Déchiffrement et décompression (unpack) ===" << std::endl;

    try {
        Packer packer("test_key_123");

        // Message chiffré et compressé simulé (à adapter selon votre implémentation)
        std::vector<unsigned char> encryptedCompressed = {
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f,
            0x72, 0x6c, 0x64, 0x21, 0x00, 0x01, 0x02, 0x03
        };

        size_t originalSize = 100;

        std::cout << "Message chiffré/compressé: ";
        printBytes(encryptedCompressed);

        std::vector<unsigned char> result = packer.unpack(encryptedCompressed, originalSize);

        std::cout << "Message déchiffré/décompressé: ";
        printBytes(result);
        std::cout << "Taille: " << result.size() << " bytes" << std::endl;
        std::cout << "✓ Test unpack OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 4 : Changement de clé
void testSetKey() {
    std::cout << "\n=== Test 4: Changement de clé ===" << std::endl;

    try {
        Packer packer("cle_initiale");
        std::cout << "Packer créé avec clé initiale" << std::endl;

        // Changement de clé simple
        packer.setKey("nouvelle_cle");
        std::cout << "✓ Changement de clé simple OK" << std::endl;

        // Changement de clé avec encodage
        packer.setKey("Y2xlX2VuY29kZWU=", KeyEncoding::BASE64);
        std::cout << "✓ Changement de clé avec encodage OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 5 : Traitement de messages vides
void testEmptyMessage() {
    std::cout << "\n=== Test 5: Message vide ===" << std::endl;

    try {
        Packer packer("test_key");
        std::vector<unsigned char> emptyMsg;

        std::vector<unsigned char> result = packer.unpack(emptyMsg, 0);

        std::cout << "Résultat pour message vide: " << result.size() << " bytes" << std::endl;
        std::cout << "✓ Test message vide OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Note: Exception attendue pour message vide - " << e.what() << std::endl;
    }
}

// Test 6 : Messages de différentes tailles
void testDifferentSizes() {
    std::cout << "\n=== Test 6: Messages de différentes tailles ===" << std::endl;

    try {
        Packer packer("size_test_key");

        // Petit message
        std::vector<unsigned char> small(10, 0xAA);
        std::vector<unsigned char> resultSmall = packer.unpack(small, 10);
        std::cout << "✓ Petit message (10 bytes) traité" << std::endl;

        // Message moyen
        std::vector<unsigned char> medium(1000, 0xBB);
        std::vector<unsigned char> resultMedium = packer.unpack(medium, 1000);
        std::cout << "✓ Message moyen (1000 bytes) traité" << std::endl;

        // Grand message
        std::vector<unsigned char> large(10000, 0xCC);
        std::vector<unsigned char> resultLarge = packer.unpack(large, 10000);
        std::cout << "✓ Grand message (10000 bytes) traité" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 7 : Multiple instances
void testMultipleInstances() {
    std::cout << "\n=== Test 7: Instances multiples ===" << std::endl;

    try {
        Packer packer1("key1");
        Packer packer2("key2");
        Packer packer3("key3", KeyEncoding::BASE64);

        std::cout << "✓ Création de 3 instances simultanées OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 8 : unpackToString (retourne un string)
void testUnpackToString() {
    std::cout << "\n=== Test 8: unpackToString (string) ===" << std::endl;

    try {
        Packer packer("string_key");

        std::vector<unsigned char> data = {
            0x48, 0x65, 0x6c, 0x6c, 0x6f  // "Hello"
        };

        std::string resultStr = packer.unpackToString(data, 5);

        std::cout << "Résultat string: \"" << resultStr << "\"" << std::endl;
        std::cout << "Longueur: " << resultStr.length() << " chars" << std::endl;
        std::cout << "✓ Test unpackToString OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 9 : Comparaison unpack vs unpackToString
void testUnpackVsUnpackToString() {
    std::cout << "\n=== Test 9: Comparaison unpack vs unpackToString ===" << std::endl;

    try {
        Packer packer("compare_key");

        std::vector<unsigned char> data = {
            0x54, 0x65, 0x73, 0x74, 0x20, 0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65
        };

        // Test unpack (retourne vecteur)
        std::vector<unsigned char> resultVec = packer.unpack(data, 50);
        std::cout << "unpack() retourne: vecteur de " << resultVec.size() << " bytes" << std::endl;

        // Test unpackToString (retourne string)
        std::string resultStr = packer.unpackToString(data, 50);
        std::cout << "unpackToString() retourne: string de " << resultStr.length() << " chars" << std::endl;

        std::cout << "✓ Test de comparaison OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Test 10 : Test avec différentes clés
void testDifferentKeys() {
    std::cout << "\n=== Test 10: Test avec différentes clés ===" << std::endl;

    try {
        std::vector<unsigned char> data = {
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46
        };

        // Test avec différentes clés
        Packer packer1("key_short");
        std::vector<unsigned char> r1 = packer1.unpack(data, 20);
        std::cout << "✓ Clé courte OK" << std::endl;

        Packer packer2("une_cle_beaucoup_plus_longue_pour_tester");
        std::vector<unsigned char> r2 = packer2.unpack(data, 20);
        std::cout << "✓ Clé longue OK" << std::endl;

        Packer packer3("Clé@avec#Caractères$Spéciaux!");
        std::vector<unsigned char> r3 = packer3.unpack(data, 20);
        std::cout << "✓ Clé avec caractères spéciaux OK" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Erreur: " << e.what() << std::endl;
    }
}

// Fonction principale de test
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   TESTS DE LA CLASSE PACKER" << std::endl;
    std::cout << "========================================" << std::endl;

    testConstructorBasic();
    testConstructorWithEncoding();
    testUnpackSimple();
    testSetKey();
    testEmptyMessage();
    testDifferentSizes();
    testMultipleInstances();
    testUnpackToString();
    testUnpackVsUnpackToString();
    testDifferentKeys();

    std::cout << "\n========================================" << std::endl;
    std::cout << "   FIN DES TESTS" << std::endl;
    std::cout << "========================================\n" << std::endl;

    return 0;
}