#include <string>    // Pour std::string
#include <iostream>  // Pour l'exemple d'utilisation
#include <limits>    // Pour numeric_limits (utilisé pour ROR/ROL par bitset si on l'avait)

// --- Fonctions utilitaires de rotation de bits (ROR et ROL) ---
// Ces fonctions ne sont pas natives en C++98, on doit les implémenter.
// Elles sont génériques pour tout type non signé.

// ROL (Rotate Left)
// Déplace les bits de 'value' vers la gauche de 'count' positions.
// Les bits sortis de gauche sont réintroduits à droite.
template <typename T>
T rotr(T value, int count) ;

template <typename T>
T rotl(T value, int count) {
    // S'assure que count est dans les limites du nombre de bits du type T
    const int num_bits = std::numeric_limits<T>::digits;
    count %= num_bits; // Utilise le modulo pour gérer les counts > num_bits

    if (count == 0) return value; // Pas de rotation
    if (count < 0) return rotr(value, -count); // Rotation gauche = rotation droite de -count

    // Formule pour la rotation gauche
    return (value << count) | (value >> (num_bits - count));
}

// ROR (Rotate Right)
// Déplace les bits de 'value' vers la droite de 'count' positions.
// Les bits sortis de droite sont réintroduits à gauche.
template <typename T>
T rotr(T value, int count) {
    // S'assure que count est dans les limites du nombre de bits du type T
    const int num_bits = std::numeric_limits<T>::digits;
    count %= num_bits; // Utilise le modulo pour gérer les counts > num_bits

    if (count == 0) return value; // Pas de rotation
    if (count < 0) return rotl(value, -count); // Rotation droite = rotation gauche de -count

    // Formule pour la rotation droite
    return (value >> count) | (value << (num_bits - count));
}

// --- Notre fonction de hachage personnalisée ---
// Conçue pour des chaînes de caractères (const char* ou std::string)
unsigned int custom_hash(const std::string& key) {
    unsigned int hash = 5381; // Initialisation comme DJB2
    const char* str = key.c_str(); // Obtenir un C-string pour itérer

    // DJB2 est hash = hash * 33 + c
    // Nous allons intégrer ROL/ROR et XOR

    for (int i = 0; str[i] != '\0'; ++i) {
        unsigned char c = static_cast<unsigned char>(str[i]); // Traiter le caractère comme non signé

        // Mécanique 1: DJB2 (multiplication par 33)
        // Ceci est une version optimisée de (hash * 33)
        hash = ((hash << 5) + hash); // hash * 32 + hash = hash * 33

        // Mécanique 2: Ajout du caractère
        hash += c;

        // Mécanique 3: Mélange avec ROR/ROL et XOR
        // On alterne les rotations et les XOR pour plus de diffusion
        if (i % 2 == 0) { // Pour les caractères pairs, rotation à gauche
            hash = rotl(hash, 7); // Rotation gauche de 7 bits
            hash ^= (c << 3);     // XOR avec le caractère décalé
        } else { // Pour les caractères impairs, rotation à droite
            hash = rotr(hash, 5); // Rotation droite de 5 bits
            hash ^= (c >> 2);     // XOR avec le caractère décalé
        }

        // Mécanique 4: Petit mélange supplémentaire avec une constante magique
        hash ^= 0xDEADBEEF; // Constante pour perturber davantage le hachage
    }

    return hash;
}

// Surcharge pour les const char*
unsigned int custom_hash(const char* key) {
    // Si l'entrée est NULL, retourne 0 ou une autre valeur par défaut
    if (!key) {
        return 0;
    }
    std::string s(key); // Convertit en std::string pour appeler l'autre fonction
    return custom_hash(s);
}


// // --- Exemple d'utilisation ---
// #ifndef NO_HASH_MAIN
// int main() {
//     std::cout << "Test de la fonction custom_hash (C++98 compatible):" << std::endl;
//     std::cout << "---------------------------------------------------" << std::endl;
//
//     std::string s1 = "hello world";
//     unsigned int hash1 = custom_hash(s1);
//     std::cout << "Hash de \"" << s1 << "\": " << hash1 << std::endl; // Utilise << pour les unsigned int
//
//     std::string s2 = "Hello World";
//     unsigned int hash2 = custom_hash(s2);
//     std::cout << "Hash de \"" << s2 << "\": " << hash2 << std::endl;
//
//     std::string s3 = "test exemple";
//     unsigned int hash3 = custom_hash(s3);
//     std::cout << "Hash de \"" << s3 << "\": " << hash3 << std::endl;
//
//     std::string s4 = "test exemple_"; // Juste un petit changement
//     unsigned int hash4 = custom_hash(s4);
//     std::cout << "Hash de \"" << s4 << "\": " << hash4 << std::endl;
//
//     // Test avec const char*
//     const char* c_str1 = "another string";
//     unsigned int hash_c1 = custom_hash(c_str1);
//     std::cout << "Hash de \"" << c_str1 << "\": " << hash_c1 << std::endl;
//
//     const char* c_str2 = "another string "; // Avec un espace
//     unsigned int hash_c2 = custom_hash(c_str2);
//     std::cout << "Hash de \"" << c_str2 << "\": " << hash_c2 << std::endl;
//
//     std::cout << "---------------------------------------------------" << std::endl;
//     std::cout << "Fin des tests." << std::endl;
//
//     return 0;
// }
// #endif // NO_HASH_MAIN