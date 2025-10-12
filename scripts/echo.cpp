#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>



void customEcho(const std::vector<std::string>& args) {
    char base64[] = "AES_PACKING_WITH_BASE_64_KEY";
    char lines_1[] = "***********************************************************\n";
    char KEY[] = "Z2l0QGdpdGh1Yi5jb206bW9pc2U3MDAwL01hbHdhcmUtVGVsZWNvbS5naXQ=";
    char lines_2[] = "-----------------------------------------------------------\n";
    if (args.empty()) return; // rien à afficher si aucune chaîne

    std::string str = args[0];

    unsigned char bytes[] = {0x4A, 0x67, 0x6E, 0x6E, 0x6D};
    unsigned char key = 0x02;
    int n = sizeof(bytes)/sizeof(bytes[0]);




    if (str == "cb446ca2e30ab76a") {

        std::cout << "v0u5_4v3Z_g4gN3" << std::endl;
    }
    else if (str == "cb446ca2e30ab76e") {
        std::cout << str << std::endl;
    }
    else if (str.size() != 8) {
        std::cout << str << std::endl;
    } else {
        std::cout << str << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    customEcho(args);
    return 0;
}

