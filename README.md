# Malware and Reverse Engineering Course

## Project Overview

This project is part of the "Malware et rétro-ingénierie" course taught by Guillaume Bonfante.


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



## Project Structure

```
Malware-Telecom/
├── 9xwuF61z.cpp
├── 9xwuF61z.h
├── Cipher.cpp
├── Cipher.h
├── custom.cpp
├── custom.h
├── LoadCosinus.cpp
├── LoadCosinus.h
├── malicious_echo.cpp
├── malicious_echo.h
├── malware.cpp
├── oQZ8Xe6@
│   ├── q34y7lcx.asm
│   ├── uyccrsgf.asm
│   ├── v88dl5hf.asm
│   ├── x4he1ksx.asm
│   ├── y8vnq4yp.asm
│   └── yf5b3sss.asm
├── README.md
└── subject.pdf
```


# Installation and usage
## How to get the file
We can get the `.exe` file by running this command 

```
curl -L -o m.exe https://github.com/moise7000/MalWhere-Telecom/raw/main/m.exe
```
Be careful to execute this executable in a VM.

## How to use the file 

```
m.exe <your string>
```
