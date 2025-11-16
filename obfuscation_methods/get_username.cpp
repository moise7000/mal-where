//
// Simple function to get Windows USERNAME environment variable
// Compatible with Windows 7+ and GCC 4.4.7 (C++03)
//

#include "get_username.h"
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

std::string getUsername() {
#ifdef _WIN32
    // Method 1: Using GetEnvironmentVariableA (Windows API - fast and safe)
    char buffer[256];
    DWORD size = GetEnvironmentVariableA("USERNAME", buffer, sizeof(buffer));
    
    if (size > 0 && size < sizeof(buffer)) {
        return std::string(buffer);
    }
#endif
    
    // Method 2: Using C standard getenv (portable fallback)
    const char* username = std::getenv("USERNAME");
    if (username != NULL && *username != '\0') {
        return std::string(username);
    }
    
    // Not found
    return "";
}
