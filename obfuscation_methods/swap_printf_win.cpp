// ============================================================================
//  Swap printf <-> OutputDebugStringA (Version Windows 7+, C++03)
//  Sur Windows, on échange printf avec OutputDebugStringA
//  Compatible avec GCC 4.4.7 (MinGW) et MSVC 2010+
// ============================================================================

#ifdef _WIN32

#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

// Function declarations
void init_printf_swap();
int swapped_printf(const char* format, ...);
void swapped_OutputDebugStringA(const char* message);

// Pointeurs vers les vraies fonctions originales
namespace {
    typedef int (*printf_func_t)(const char*, ...);
    typedef void (WINAPI *OutputDebugStringA_func_t)(LPCSTR);
    
    // On garde des références vers les vraies fonctions
    static printf_func_t original_printf = NULL;
    static OutputDebugStringA_func_t original_OutputDebugStringA = NULL;
    static bool swap_initialized = false;
}

// Initialize: save original function pointers
void init_printf_swap() {
    if (swap_initialized) return;
    
    // On sauvegarde les pointeurs originaux
    // Note: Sous Windows sans hooking complexe, on utilise directement les fonctions
    original_printf = &::printf;
    original_OutputDebugStringA = &::OutputDebugStringA;
    
    swap_initialized = true;
}

// Swapped printf: appelle OutputDebugStringA au lieu de printf
int swapped_printf(const char* format, ...) {
    if (!swap_initialized) init_printf_swap();
    
    // Format le message
    char buffer[4096];
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);
    
    if (result < 0) return result;
    
    buffer[sizeof(buffer) - 1] = '\0';
    
    // Au lieu d'utiliser printf, on envoie vers OutputDebugStringA
    if (original_OutputDebugStringA) {
        original_OutputDebugStringA(buffer);
    }
    
    return result;
}

// Swapped OutputDebugStringA: appelle printf au lieu de OutputDebugStringA
void swapped_OutputDebugStringA(const char* message) {
    if (!swap_initialized) init_printf_swap();
    
    // Au lieu d'utiliser OutputDebugStringA, on utilise printf
    if (original_printf) {
        original_printf("%s", message);
    }
}

#endif // _WIN32
