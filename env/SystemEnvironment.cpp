//
// Created by ewan decima on 11/5/25.
//

#include "SystemEnvironment.h"
#include <Windows.h>

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

}
