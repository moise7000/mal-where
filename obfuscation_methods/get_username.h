//
// Simple function to get Windows USERNAME environment variable
// Compatible with Windows 7+ and GCC 4.4.7 (C++03)
//

#ifndef GET_USERNAME_H
#define GET_USERNAME_H

#include <string>

// Get Windows USERNAME environment variable
// Returns the username as a string, or empty string if not found
std::string getUsername();

#endif // GET_USERNAME_H
