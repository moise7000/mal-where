//
// Created by ewan decima on 11/5/25.
//

#include "SystemEnvironment.h"
#include <Windows.h>
#include <cstdlib>

namespace systemEnvironment {

    std::string getProcessorArchitecture() {
        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);

        switch (systemInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64:
                return "x64";
            case PROCESSOR_ARCHITECTURE_INTEL:
                return "x86";
            case PROCESSOR_ARCHITECTURE_IA64:
                return "IA64";
            default:
                return "Unknown";
        }
    }


    std::string getTempPath() {
        char buffer[MAX_PATH + 1];
        DWORD length = GetTempPathA(MAX_PATH + 1, buffer);

        if (length > 0 && length <= MAX_PATH) {
            return std::string(buffer);
        }

        return "";
    }

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
        const char* username = getenv("USERNAME");
        if (username != NULL && *username != '\0') {
            return std::string(username);
        }

        // Not found
        return "";
    }

    std::string getName() {
        char buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(buffer);

        if (GetComputerNameA(buffer, &size)) {
            return std::string(buffer);
        }

        return "";
    }



}
