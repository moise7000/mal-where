//
// Created by ewan decima on 10/30/25.
//

#ifndef MAL_WHERE_TESTINGTOOLS_H
#define MAL_WHERE_TESTINGTOOLS_H

#include <vector>
#include <string>


namespace TestingTools {


    /**
     * Affiche les données en format hexadécimal avec un label.
     * Les données sont affichées par blocs de 16 bytes par ligne.
     *
     * @param data Le vecteur de bytes à afficher
     * @param label Le texte descriptif à afficher avant les données
     */
    void printHex(const std::vector<unsigned char>& data, const std::string& label);


    /**
     * Affiche les 20 premiers bytes d'un vecteur en format décimal.
     * Si le vecteur contient plus de 20 bytes, affiche "..." suivi du nombre total.
     *
     * @param data Le vecteur de bytes à afficher
     */
    void printBytes(const std::vector<unsigned char>& data);


    /**
     * Affiche un tableau de bytes en format hexadécimal avec un label.
     * Chaque byte est affiché au format 0xXX, séparés par des virgules.
     *
     * @param label Le texte descriptif à afficher avant les données
     * @param data Le pointeur vers le tableau de bytes
     * @param size La taille du tableau en bytes
     */
    void printBytes(const std::string& label, const unsigned char* data, size_t size);


    /**
     * Affiche un vecteur de bytes en format hexadécimal avec un label.
     * Chaque byte est affiché au format 0xXX, séparés par des virgules.
     *
     * @param label Le texte descriptif à afficher avant les données
     * @param data Le vecteur de bytes à afficher
     */
    void printVector(const std::string& label, const std::vector<unsigned char>& data);


    /**
     * Compare deux vecteurs de bytes pour vérifier leur égalité.
     * Vérifie d'abord la taille, puis compare byte par byte.
     *
     * @param v1 Le premier vecteur à comparer
     * @param v2 Le second vecteur à comparer
     * @return true si les vecteurs sont identiques, false sinon
     */
    bool compareVectors(const std::vector<unsigned char>& v1, const std::vector<unsigned char>& v2);


    /**
     * Affiche un message en vert dans la console (Windows uniquement).
     * La couleur est automatiquement restaurée après l'affichage.
     *
     * @param message Le message à afficher en vert
     */
    void printGreen(const std::string& message);


    /**
     * Affiche un message en rouge dans la console (Windows uniquement).
     * La couleur est automatiquement restaurée après l'affichage.
     *
     * @param message Le message à afficher en rouge
     */
    void printRed(const std::string& message);


    /**
     * Affiche un message en jaune dans la console (Windows uniquement).
     * La couleur est automatiquement restaurée après l'affichage.
     *
     * @param message Le message à afficher en jaune
     */
    void printYellow(const std::string& message);

    std::string stringToBinary(const std::string &input);

    std::string binaryToString(const std::string &input);gi

}




#endif //MAL_WHERE_TESTINGTOOLS_H