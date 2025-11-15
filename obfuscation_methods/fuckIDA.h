//
// Created by ewan decima on 11/14/25.
//

#ifndef MAL_WHERE_FUCKIDA_H
#define MAL_WHERE_FUCKIDA_H

#include <cstdint>
#include <cstddef>

// Type générique de fonction générée
using JmpChainFunc = void(*)();

/**
* Génère en mémoire exécutable une longue chaîne de JMP rel32.
* @param N : nombre de sauts à générer
* @return : pointeur vers la fonction générée, ou nullptr en cas d'erreur
*/
JmpChainFunc generateJumpChain(size_t N);

#endif //MAL_WHERE_FUCKIDA_H