#include <cstdlib>  // For getenv
#include <string>   // For std::string
#include <iostream> // For example usage

// Function to get the value of the %OS% environment variable
// Returns an empty std::string if the variable is not found or is empty.
std::string get_os_environment_variable() {
    // getenv returns a const char* or NULL if the variable is not set.
    const char* os_value_c_str = std::getenv("OS");

    if (os_value_c_str != NULL) {
        // If the variable is found, convert the C-string to std::string.
        // This makes a copy of the string, which is good practice as
        // the pointer returned by getenv should not be modified.
        return std::string(os_value_c_str);
    } else {
        // If the variable is not found, return an empty string.
        return std::string("");
    }
}

// --- Example Usage ---
int main() {
    std::cout << "Retrieving the %OS% environment variable (C++98 compatible):" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    std::string os_env_value = get_os_environment_variable();

    if (!os_env_value.empty()) {
        std::cout << "The value of %OS% is: \"" << os_env_value << "\"" << std::endl;
    } else {
        std::cout << "The %OS% environment variable is not set or is empty on this system." << std::endl;
            
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "End of test." << std::endl;

    return 0;
}