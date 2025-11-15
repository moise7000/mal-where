//
// Created by ewan decima on 11/15/25.
//

#include "../obfuscation_methods/fuckIDA.h"
#include <iostream>

int main() {
    size_t N = 500000; // Tu peux augmenter à 100k si nécessaire
    JmpChainFunc func = generateJumpChain(N);

    if(func == NULL) {
        std::cerr << "Failed to generate jump chain\n";
        return 1;
    }

    std::cout << "Jump chain generated at: " << (void*)func << "\n";

    func();

    return 0;
}
