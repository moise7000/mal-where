//
// Created by ewan decima on 10/30/25.
//

#ifndef MAL_WHERE_TESTINGTOOLS_H
#define MAL_WHERE_TESTINGTOOLS_H

#include <vector>
#include <string>


namespace TestingTools {


    /**
    *
    */
    void printHex(const std::vector<unsigned char>& data, const std::string& label);


    /**
    *
    */
    void printBytes(const std::vector<unsigned char>& data);


    void printBytes(const std::string& label, const unsigned char* data, size_t size);

    /**
    *
    */
    void printVector(const std::string& label, const std::vector<unsigned char>& data);


    /**
    *
    */
    bool compareVectors(const std::vector<unsigned char>& v1, const std::vector<unsigned char>& v2);

    /**
    *
    */
    void printGreen(const std::string& message);

    /**
    *
    */
    void printRed(const std::string& message);

    /**
    *
    */
    void printYellow(const std::string& message);

}




#endif //MAL_WHERE_TESTINGTOOLS_H