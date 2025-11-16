//
// Created by ewan decima on 11/16/25.
//


#include "../devil/Rickroll.h"

int main(int argc, char* argv[]) {
    // Vérifie si on est dans le nouveau terminal
    rollFromCommandLine(argc, argv);

    // Sinon, ouvre un nouveau terminal
    if (argc == 1) {
        rollInNewTerminal(100);
    }

    return 0;
}