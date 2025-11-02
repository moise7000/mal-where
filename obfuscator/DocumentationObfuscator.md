# C++ Code Obfuscator 

A powerful tool to obfuscate C++ code with multiple obfuscation methods while maintaining code compilability. Supports consistent obfuscation across multiple files through a mapping system.




##  Quick Start

### Obfuscate a single file

```bash
python3 obfuscator.py my_program.cpp
```

Output: `my_program_obfuscated.cpp`

### Obfuscate multiple files

```bash
python3 obfuscator.py file1.cpp file2.cpp file3.cpp
```

### With mapping persistence

```bash
python3 obfuscator.py *.cpp --mapping project.json
```

## 🛠️ Obfuscation Methods

The script provides 7 obfuscation methods:

| Method | Description | Example |
|---------|-------------|---------|
| `variables` | Rename variables | `count` → `v_a8Kj2x9L` |
| `functions` | Rename functions | `calculateSum` → `f_X3hT8vBn` |
| `junk` | Add useless code | `int __junk_1234 = 0;` |
| `conditions` | Insert fake conditions | `if(1) { ... }` |
| `comments` | Replace comments | `// System initialization` |
| `strings` | Obfuscate strings | `"Hello"` → `(char[]){0x48,0x65,...}` |
| `arithmetic` | Obfuscate numbers | `42` → `(84 / 2)` |

### Select specific methods

```bash
# Only rename variables and functions
python3 obfuscator.py code.cpp -m variables functions

# All methods except arithmetic
python3 obfuscator.py code.cpp -m variables functions junk conditions comments strings

# All methods (default)
python3 obfuscator.py code.cpp -m all

```

## Examples

```
# Case 1: First pass - creates the mapping
python3 obfuscator.py main.cpp utils.cpp --save-mapping mapping.json

# Case 2: Add more files with the same mapping
python3 obfuscator.py network.cpp database.cpp --load-mapping mapping.json --save-mapping mapping.json

# Case 3: Simplified version (load AND save)
python3 obfuscator.py *.cpp --mapping project_mapping.json

# Case 4: With specific methods
python3 obfuscator.py *.cpp --mapping map.json -m variables functions strings
```

## Mapping Management

The mapping system allows you to obfuscate multiple files consistently: the same variable/function names will have the same obfuscated names across all files.

### Create a new mapping

```bash
# Obfuscate and save the mapping
python3 obfuscator.py main.cpp utils.cpp --save-mapping mapping.json
```

### Use an existing mapping

```bash
# Load the mapping to obfuscate other files
python3 obfuscator.py network.cpp database.cpp --load-mapping mapping.json
```

### Load AND save (recommended)

```bash
# Use --mapping to load existing mapping and update it
python3 obfuscator.py *.cpp --mapping project.json
```




##  Complete Options

```bash
python3 obfuscator.py [OPTIONS] file1.cpp [file2.cpp ...]
```

### Positional arguments

- `input` : One or more C++ files to obfuscate

### Options

| Option | Description |
|--------|-------------|
| `-o`, `--output-dir DIR` | Output directory for obfuscated files |
| `-m`, `--methods METHOD [METHOD ...]` | Obfuscation methods to apply |
| `--mapping FILE` | JSON file to load AND save the mapping |
| `--save-mapping FILE` | Save the mapping to this file |
| `--load-mapping FILE` | Load the mapping from this file |
| `-h`, `--help` | Show help message |

## Mapping Structure

The mapping file is a JSON containing three sections:

```json
{
  "variables": {
    "count": "v_a8Kj2x9L",
    "index": "v_zP4mN6qR",
    "total": "v_K3nY8wLm"
  },
  "functions": {
    "calculateSum": "f_X3hT8vBn",
    "processData": "f_M9wY2kLp",
    "initSystem": "f_Q7rZ3xPt"
  },
  "strings": {
    "Hello World": "(char[]){0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0}",
    "Error": "(char[]){0x45,0x72,0x72,0x6f,0x72,0}"
  }
}
```





## Complete Example

### Original file: `main.cpp`

```cpp
#include <iostream>

int calculateSum(int a, int b) {
    int result = a + b;
    return result;
}

int main() {
    int x = 5;
    int y = 10;
    std::cout << "Result: " << calculateSum(x, y) << std::endl;
    return 0;
}
```

### Command

```bash
python3 obfuscator.py main.cpp --mapping test.json
```

### Obfuscated file: `main_obfuscated.cpp` (example)

```cpp
#include <iostream>

int f_X3hT8vBn(int v_a8Kj2x9L, int v_zP4mN6qR) {
    int v_K3nY8wLm = v_a8Kj2x9L + v_zP4mN6qR;
    int __junk_1234 = 0;
    return v_K3nY8wLm;
}

int main() {
    // System initialization
    int v_Q7rZ3xPt = (10 / 2);
    int v_M9wY2kLp = (20 / 2);
    std::cout << (char[]){0x52,0x65,0x73,0x75,0x6c,0x74,0x3a,0x20,0} << f_X3hT8vBn(v_Q7rZ3xPt, v_M9wY2kLp) << std::endl;
    return 0;
}
```




