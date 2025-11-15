//
// Created by ewan decima on 10/30/25.
//

#include "TestingTools.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cassert>
#include <windows.h>


using namespace std;

namespace TestingTools {

    void printHex(const std::vector<unsigned char>& data, const std::string& label) {
        std::cout << label << " (" << data.size() << " bytes):\n";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(data[i]) << " ";
            if ((i + 1) % 16 == 0) std::cout << "\n";  // Nouvelle ligne tous les 16 bytes
        }
        std::cout << std::dec << "\n\n";  // Retour en décimal
    }


    void printBytes(const std::vector<unsigned char>& data) {
        std::cout << "[ ";
        for (size_t i = 0; i < data.size() && i < 20; ++i) {
            std::cout << static_cast<int>(data[i]) << " ";
        }
        if (data.size() > 20) {
            std::cout << "... (" << data.size() << " bytes total)";
        }
        std::cout << " ]" << std::endl;
    }


    void printBytes(const std::string& label, const unsigned char* data, size_t size) {
        std::cout << label << " ";
        for (size_t i = 0; i < size; i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(data[i]);
            if (i < size - 1) std::cout << ", ";
        }
        std::cout << std::dec << std::endl;
    }


    void printVector(const std::string& label, const std::vector<unsigned char>& data){
        std::cout << label << " ";
        for (size_t i = 0; i < data.size(); i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(data[i]);
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << std::dec << std::endl;
    }


    bool compareVectors(const std::vector<unsigned char>& v1, const std::vector<unsigned char>& v2){
        if (v1.size() != v2.size()) {
            return false;
        }
        for (size_t i = 0; i < v1.size(); ++i) {
            if (v1[i] != v2[i]) {
                return false;
            }
        }
        return true;
    }


    void printGreen(const std::string& message){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 10);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
    }

    void printRed(const std::string& message){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
    }


    void printYellow(const std::string& message) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 14);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
    }




    string stringToBinary(const std::string &input) {
        std::string result;
        for (size_t i = 0; i < input.size(); ++i) {
            unsigned char c = input[i];
            for (int j = 7; j >= 0; --j) {
                result += ((c >> j) & 1) ? '1' : '0';
            }
        }
        return result;
    }

    string binaryToString(const std::string &input) {
        std::string result;
        if (input.size() % 8 != 0) {
            // La longueur doit être un multiple de 8
            return "";
        }

        for (size_t i = 0; i < input.size(); i += 8) {
            unsigned char c = 0;
            for (int j = 0; j < 8; ++j) {
                if (input[i + j] == '1') {
                    c |= (1 << (7 - j));
                } else if (input[i + j] != '0') {
                    // caractère invalide
                    return "";
                }
            }
            result += c;
        }
        return result;
    }




}