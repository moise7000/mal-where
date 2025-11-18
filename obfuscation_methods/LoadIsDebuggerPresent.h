//
// Created by ewan decima on 11/18/25.
//

#ifndef MAL_WHERE_LOADISDEBUGGERPRESENT_H
#define MAL_WHERE_LOADISDEBUGGERPRESENT_H



#include <Windows.h>

// Type de la fonction IsDebuggerPresent
typedef BOOL (WINAPI *type_IsDebuggerPresent)();

// Charge IsDebuggerPresent dynamiquement
type_IsDebuggerPresent LoadIsDebuggerPresentFunction();

// Libère la bibliothèque kernel32.dll
void UnloadIsDebuggerPresentFunction();




#endif //MAL_WHERE_LOADISDEBUGGERPRESENT_H