//
// Created by ewan decima on 11/15/25.
//

#ifndef MAL_WHERE_LOADPRINTFFUNCTION_H
#define MAL_WHERE_LOADPRINTFFUNCTION_H


#include <windows.h>

// Type pour la fonction printf
typedef int (*type_printf)(const char *, ...);

// Declaration des fonctions
type_printf LoadPrintfFunction();
void UnloadPrintfFunction();

#endif //MAL_WHERE_LOADPRINTFFUNCTION_H