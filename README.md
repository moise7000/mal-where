# Malware and Reverse Engineering Course

## Project Overview

This project is part of the "Malware et rétro-ingénierie" course taught by Guillaume Bonfante. 

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



## Project Structure

```
project/
├── README.md           # This file
├── malicious           # The executable
├── presentation.pdf    # Oral presentation slides
└── analysis.pdf        # 1-2 page analysis document
```


## Notes

- This is a Goodware implementation - it performs no malicious actions
- The program does not modify system state
- No anti-debugging or obfuscation techniques are implemented
- Straightforward, transparent implementation
