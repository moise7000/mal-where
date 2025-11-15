//
// Created by ewan decima on 11/15/25.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <stddef.h>

// Types Windows pour compatibilité cross-platform
#ifdef _WIN32
    #include <windows.h>
#else
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;
#endif

// Structures PE simplifiées
#pragma pack(push, 1)
struct DOS_HEADER {
    WORD e_magic;      // "MZ"
    BYTE e_rest[58];
    DWORD e_lfanew;    // Offset vers PE header
};

struct COFF_HEADER {
    WORD Machine;
    WORD NumberOfSections;
    DWORD TimeDateStamp;
    DWORD PointerToSymbolTable;
    DWORD NumberOfSymbols;
    WORD SizeOfOptionalHeader;
    WORD Characteristics;
};

struct OPTIONAL_HEADER_32 {
    WORD Magic;
    BYTE MajorLinkerVersion;
    BYTE MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    DWORD BaseOfData;
    DWORD ImageBase;
    // ... autres champs
};
#pragma pack(pop)

// Macro offsetof pour GCC 4.4.7 si nécessaire
#ifndef offsetof
    #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

class PEFuzzer {
private:
    std::vector<BYTE> peData;
    std::string filename;
    DWORD peOffset;

    bool loadFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << " Impossible d'ouvrir: " << path << std::endl;
            return false;
        }

        peData = std::vector<BYTE>((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
        file.close();

        // Vérifier signature DOS
        if (peData.size() < sizeof(DOS_HEADER)) return false;
        DOS_HEADER* dos = (DOS_HEADER*)peData.data();
        if (dos->e_magic != 0x5A4D) { // "MZ"
            std::cerr << " Pas un fichier PE valide (signature MZ manquante)" << std::endl;
            return false;
        }

        peOffset = dos->e_lfanew;

        // Vérifier signature PE
        if (peOffset + 4 > peData.size()) return false;
        DWORD* peSig = (DWORD*)(peData.data() + peOffset);
        if (*peSig != 0x00004550) { // "PE\0\0"
            std::cerr << " Signature PE invalide" << std::endl;
            return false;
        }

        std::cout << " PE chargé: " << peData.size() << " bytes" << std::endl;
        return true;
    }

    bool saveFile(const std::string& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << " Impossible d'écrire: " << path << std::endl;
            return false;
        }
        file.write((char*)peData.data(), peData.size());
        file.close();
        std::cout << " Sauvegardé: " << path << std::endl;
        return true;
    }

    template<typename T>
    void writeValue(DWORD offset, T value) {
        if (offset + sizeof(T) > peData.size()) {
            std::cerr << " Offset hors limites: 0x" << std::hex << offset << std::endl;
            return;
        }
        *(T*)(peData.data() + offset) = value;
    }

    template<typename T>
    T readValue(DWORD offset) {
        if (offset + sizeof(T) > peData.size()) return 0;
        return *(T*)(peData.data() + offset);
    }

public:
    bool load(const std::string& path) {
        filename = path;
        return loadFile(path);
    }

    bool save(const std::string& path) {
        return saveFile(path);
    }

    // Corruption du COFF Header
    void corruptNumberOfSections(WORD value) {
        DWORD offset = peOffset + 4 + offsetof(COFF_HEADER, NumberOfSections);
        WORD original = readValue<WORD>(offset);
        writeValue<WORD>(offset, value);
        std::cout << " NumberOfSections: " << original << " → " << value << std::endl;
    }

    void corruptSizeOfOptionalHeader(WORD value) {
        DWORD offset = peOffset + 4 + offsetof(COFF_HEADER, SizeOfOptionalHeader);
        WORD original = readValue<WORD>(offset);
        writeValue<WORD>(offset, value);
        std::cout << " SizeOfOptionalHeader: " << original << " → " << value << std::endl;
    }

    // Corruption de l'Optional Header
    void corruptSizeOfCode(DWORD value) {
        DWORD offset = peOffset + 4 + sizeof(COFF_HEADER) + offsetof(OPTIONAL_HEADER_32, SizeOfCode);
        DWORD original = readValue<DWORD>(offset);
        writeValue<DWORD>(offset, value);
        std::cout << " SizeOfCode: 0x" << std::hex << original << " → 0x" << value << std::dec << std::endl;
    }

    void corruptEntryPoint(DWORD value) {
        DWORD offset = peOffset + 4 + sizeof(COFF_HEADER) + offsetof(OPTIONAL_HEADER_32, AddressOfEntryPoint);
        DWORD original = readValue<DWORD>(offset);
        writeValue<DWORD>(offset, value);
        std::cout << " EntryPoint: 0x" << std::hex << original << " → 0x" << value << std::dec << std::endl;
    }

    void corruptSizeOfInitializedData(DWORD value) {
        DWORD offset = peOffset + 4 + sizeof(COFF_HEADER) + offsetof(OPTIONAL_HEADER_32, SizeOfInitializedData);
        DWORD original = readValue<DWORD>(offset);
        writeValue<DWORD>(offset, value);
        std::cout << " SizeOfInitializedData: 0x" << std::hex << original << " → 0x" << value << std::dec << std::endl;
    }

    // Corruptions extrêmes
    void corruptPESignature() {
        writeValue<DWORD>(peOffset, 0x45454545); // "EEEE"
        std::cout << " PE Signature corrompue → EEEE" << std::endl;
    }

    void corruptDOSStub() {
        for (size_t i = 64; i < peOffset && i < peData.size(); i++) {
            peData[i] = 0xCC; // INT3 breakpoint
        }
        std::cout << "🔧 DOS Stub rempli de 0xCC (INT3)" << std::endl;
    }

    void displayInfo() {
        std::cout << "\n Informations PE:" << std::endl;
        std::cout << "   PE Offset: 0x" << std::hex << peOffset << std::dec << std::endl;

        DWORD coffOffset = peOffset + 4;
        COFF_HEADER* coff = (COFF_HEADER*)(peData.data() + coffOffset);
        std::cout << "   NumberOfSections: " << coff->NumberOfSections << std::endl;
        std::cout << "   SizeOfOptionalHeader: " << coff->SizeOfOptionalHeader << std::endl;

        if (coffOffset + sizeof(COFF_HEADER) + sizeof(OPTIONAL_HEADER_32) < peData.size()) {
            OPTIONAL_HEADER_32* opt = (OPTIONAL_HEADER_32*)(peData.data() + coffOffset + sizeof(COFF_HEADER));
            std::cout << "   SizeOfCode: 0x" << std::hex << opt->SizeOfCode << std::dec << std::endl;
            std::cout << "   EntryPoint: 0x" << std::hex << opt->AddressOfEntryPoint << std::dec << std::endl;
        }
    }
};

void printUsage() {
    std::cout << R"(
🔨 PE Header Fuzzer - Testeur de robustesse IDA
Usage: pe_fuzzer.exe <input.exe> <output.exe> [options]

Options de corruption:
  -sections <n>        Modifier NumberOfSections (ex: -sections 999)
  -optsize <n>         Modifier SizeOfOptionalHeader (ex: -optsize 0)
  -codesize <hex>      Modifier SizeOfCode (ex: -codesize FFFFFFFF)
  -entrypoint <hex>    Modifier AddressOfEntryPoint (ex: -entrypoint DEADBEEF)
  -initdata <hex>      Modifier SizeOfInitializedData (ex: -initdata 99999999)
  -corrupt-pe          Corrompre la signature PE
  -corrupt-dos         Remplir le DOS stub de INT3
  -info                Afficher les infos sans modifier

Exemples:
  pe_fuzzer.exe calc.exe calc_fuzzy.exe -sections 100
  pe_fuzzer.exe test.exe test_crash.exe -codesize FFFFFFFF -sections 50
  pe_fuzzer.exe app.exe app_info.exe -info
)" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << " PE Header Fuzzer v1.0\n" << std::endl;

    if (argc < 3) {
        printUsage();
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    PEFuzzer fuzzer;
    if (!fuzzer.load(inputFile)) {
        return 1;
    }

    bool modified = false;
    bool infoOnly = false;

    // Parser les arguments
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-info") {
            infoOnly = true;
        }
        else if (arg == "-sections" && i + 1 < argc) {
            fuzzer.corruptNumberOfSections(std::atoi(argv[++i]));
            modified = true;
        }
        else if (arg == "-optsize" && i + 1 < argc) {
            fuzzer.corruptSizeOfOptionalHeader(std::atoi(argv[++i]));
            modified = true;
        }
        else if (arg == "-codesize" && i + 1 < argc) {
            fuzzer.corruptSizeOfCode(std::strtoul(argv[++i], nullptr, 16));
            modified = true;
        }
        else if (arg == "-entrypoint" && i + 1 < argc) {
            fuzzer.corruptEntryPoint(std::strtoul(argv[++i], nullptr, 16));
            modified = true;
        }
        else if (arg == "-initdata" && i + 1 < argc) {
            fuzzer.corruptSizeOfInitializedData(std::strtoul(argv[++i], nullptr, 16));
            modified = true;
        }
        else if (arg == "-corrupt-pe") {
            fuzzer.corruptPESignature();
            modified = true;
        }
        else if (arg == "-corrupt-dos") {
            fuzzer.corruptDOSStub();
            modified = true;
        }
        else {
            std::cerr << "️  Argument inconnu: " << arg << std::endl;
        }
    }

    fuzzer.displayInfo();

    if (!infoOnly && modified) {
        std::cout << "\n Sauvegarde des modifications..." << std::endl;
        if (!fuzzer.save(outputFile)) {
            return 1;
        }
        std::cout << "\n ATTENTION: Le fichier modifié peut crasher IDA!" << std::endl;
        std::cout << "   Testez avec prudence dans une VM ou un environnement isolé." << std::endl;
    } else if (!modified && !infoOnly) {
        std::cout << "\n Aucune modification spécifiée. Utilisez -info pour voir les détails." << std::endl;
    }

    return 0;
}