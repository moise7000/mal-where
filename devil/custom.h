//
// Custom function for malicious echo
// Compatible with Windows 7+ and GCC 4.4.7 (C++03)
//

#ifndef CUSTOM_H
#define CUSTOM_H

#include <string>

/**
 * @brief Custom function that checks if encrypted and triple-hashed string matches secret
 * @param str Input string to process
 */
void custom(const std::string& str);

#endif // CUSTOM_H
