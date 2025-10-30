# Malware and Reverse Engineering Course

## Project Overview

This project is part of the "Malware et rétro-ingénierie" course taught by Guillaume Bonfante. 

## Tests
You can find few tests in the folder `tests`, you can run old the tests at once by running:
```sh
build.bat tests
```
or for a single test (for exemple for testing the file `tests/test_cipher.cpp`) 
```sh
build.bat test CIPHER
```

and then delete all the executables by running 
```sh
build.bat clean
```

## Project Structure

```
├── build.bat
├── CppStandardLibFunctions.md
├── obfuscator
│   ├── CppMultiObfuscator.py
│   ├── CppObfuscator.py
│   └── MappingApplicator.py
├── packer
│   ├── Cipher.cpp
│   ├── Cipher.h
│   ├── Compressor.cpp
│   ├── Compressor.h
│   ├── ComputerName.cpp
│   ├── ComputerName.h
│   ├── Packer.cpp
│   ├── Packer.h
│   ├── Stub.cpp
│   └── Stub.h
├── Printf_assembly.asm
├── Printf.cpp
├── README.md
├── subject.pdf
└── tests
    ├── test_cipher.cpp
    ├── test_compressor.cpp
    ├── test_computer_name.cpp
    ├── test_packing_mechanism.cpp
    ├── test_stub.cpp
    ├── TestingTools.cpp
    └── TestingTools.h
```



## Evaluation Requirements
- [ ] Anti-Debug mechanism    
- [ ] Anti-Debug Checksum     
- [ ] Syntaxic obfuscation    
- [ ] Switch functions        
- [x] Packer                  
- [ ] Self modifying function 
- [ ] Attack on the VM        




## Program Specification

### Functionality
The program accepts a single command-line argument and displays it back to the user.

### Input Requirements
- **Input**: A string provided as `argv[1]`
- **Length**: Less than 8 printable ASCII characters
- **Arguments**: Exactly one argument must be provided
- **Behavior**: If input doesn't meet requirements or wrong number of arguments is provided, behavior is unspecified

### Expected Behavior
The program displays the input string provided by the user on the console.





## Notes

- This is a Goodware implementation - it performs no malicious actions
- The program does not modify system state
- No anti-debugging or obfuscation techniques are implemented
- Straightforward, transparent implementation
