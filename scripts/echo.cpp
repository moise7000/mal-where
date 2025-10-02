#include <iostream>
#include <vector>
#include <string>

void customEcho(const std::vector<std::string>& args) {
    if (args.empty()) return; // rien à afficher si aucune chaîne

    std::string str = args[0];

    if (str == "cb446ca2e30ab76a") {
        std::cout << "v0u5_4v3Z_g4gN3" << std::endl;
    }
    else if (str == "cb446ca2e30ab76e") {
        std::cout << str << std::endl;
    }
    else if (str.size() != 8) {
        std::cout << "v0u5_4v3Z_p3rDu3" << std::endl;
    } else {
        std::cout << str << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    customEcho(args);
    return 0;
}

