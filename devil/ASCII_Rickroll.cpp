//
// Created by ewan decima on 11/16/25.
//

// ASCII_Rickroll.cpp
#include "ASCII_Rickroll.h"
#include <ctime>
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

void delay(int milliseconds) {
#ifdef _WIN32
    // Version Windows optimisée
    Sleep(milliseconds);
#else
    // Version portable pour autres systèmes
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds * (CLOCKS_PER_SEC / 1000));
#endif
}

void roll(int frameTime) {
    const int line_chars = 95;
    const int lines = 35;
    const int numFrames = 149;
    int frameIndex = 0;

    for (int frame = 1; frame < numFrames; frame++) {
        // Efface l'écran pour la prochaine frame
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        for (int i = 0; i <= lines; i++) {
            for (int j = 0; j <= line_chars; j++) {
                // Affiche chaque caractère
                std::cout << frames[frameIndex];
                frameIndex++;
            }
        }

        delay(frameTime);
    }
    std::cout << std::endl;
}

// Nouvelle fonction : ouvre un nouveau terminal et lance l'animation
void rollInNewTerminal(int frameTime) {
#ifdef _WIN32
    // Obtient le chemin de l'exécutable actuel
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // Commande pour ouvrir un nouveau terminal cmd.exe
    std::string command = "start cmd /k \"";
    command += exePath;
    command += "\" rollnow ";
    command += static_cast<char>('0' + frameTime / 10);
    command += static_cast<char>('0' + frameTime % 10);

    system(command.c_str());
#else
    // Version Linux/Unix (non testé sur Windows 7)
    system("xterm -e ./rickroll &");
#endif
}

// Fonction helper pour lancer depuis la ligne de commande
void rollFromCommandLine(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "rollnow") {
        int frameTime = 100; // valeur par défaut
        if (argc > 2) {
            frameTime = atoi(argv[2]);
        }
        roll(frameTime);

        // Garde le terminal ouvert à la fin
        std::cout << "\nAppuyez sur Entree pour fermer..." << std::endl;
        std::cin.get();
    }
}