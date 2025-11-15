//
// Created by ewan decima on 11/15/25.
//

#include "../obfuscation_methods/fuckIDA.h".h""
#include <iostream>

int main() {
    size_t N = 2000; // Tu peux augmenter à 100k si nécessaire
    auto func = generateJumpChain(N);

    if(func == nullptr) {
        std::cerr << "Failed to generate jump chain\n";
        return 1;
    }

    std::cout << "Jump chain generated at: " << (void*)func << "\n";


     func();

    return 0;
}