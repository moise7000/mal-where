//
// Created by ewan decima on 11/16/25.
//

#ifndef MAL_WHERE_MALICIOUSECHO_H
#define MAL_WHERE_MALICIOUSECHO_H
#include <string>

template <typename F, typename G>
int malicious_echo(const std::string& str, F devil_code, G custom_func) {

    if (!(str.length() >= 8)) {
        // str mauvaise longueur (<8), appel de d
        devil_code();
    } else {
        // str de bonne longueur, appel de custom_func
        custom_func(str);
    }

    return 0;
}

#endif //MAL_WHERE_MALICIOUSECHO_H