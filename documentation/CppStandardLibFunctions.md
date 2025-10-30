# C++ Standard Library - Function Reference

## Input/Output

### `<iostream>`
- `cin` - Standard input
- `cout` - Standard output
- `cerr` - Standard error
- `getline()` - Read complete line

### `<cstdio>`
- `printf()` - Formatted output
- `scanf()` - Formatted input
- `fopen()` - Open file
- `fclose()` - Close file
- `fprintf()` - Formatted write to file
- `fscanf()` - Formatted read from file
- `fgets()` - Read line from file
- `fputs()` - Write line to file

### `<fstream>`
- `ifstream` - Input file stream
- `ofstream` - Output file stream
- `fstream` - Bidirectional file stream

---

## Strings

### `<string>`
- `length()` - String length
- `size()` - String size
- `substr()` - Extract substring
- `find()` - Search in string
- `replace()` - Replace characters
- `append()` - Append to end
- `compare()` - Compare strings
- `c_str()` - Convert to C string

### `<cstring>`
- `strlen()` - C string length
- `strcpy()` - Copy string
- `strcat()` - Concatenate strings
- `strcmp()` - Compare strings
- `strstr()` - Search for substring
- `memcpy()` - Copy memory
- `memset()` - Fill memory

---

## Containers

### `<vector>`
- `push_back()` - Add to end
- `pop_back()` - Remove from end
- `size()` - Number of elements
- `empty()` - Check if empty
- `clear()` - Clear vector
- `at()` - Access with bounds checking
- `front()` - First element
- `back()` - Last element

### `<list>`
- `push_front()` - Add to front
- `push_back()` - Add to back
- `pop_front()` - Remove from front
- `pop_back()` - Remove from back
- `sort()` - Sort list

### `<map>`
- `insert()` - Insert key-value pair
- `erase()` - Remove element
- `find()` - Search for key
- `count()` - Count occurrences of key
- `clear()` - Clear map

### `<set>`
- `insert()` - Insert element
- `erase()` - Remove element
- `find()` - Search for element
- `count()` - Check element presence

### `<queue>`
- `push()` - Add element
- `pop()` - Remove element
- `front()` - First element
- `back()` - Last element

### `<stack>`
- `push()` - Add element
- `pop()` - Remove element
- `top()` - Top element

---

## Mathematics

### `<cmath>`
- `sqrt()` - Square root
- `pow()` - Power
- `abs()` - Absolute value
- `sin()` - Sine
- `cos()` - Cosine
- `tan()` - Tangent
- `log()` - Natural logarithm
- `exp()` - Exponential
- `ceil()` - Round up
- `floor()` - Round down
- `round()` - Round to nearest
- `fabs()` - Absolute value (floating point)

---

## Algorithms

### `<algorithm>`
- `sort()` - Sort container
- `reverse()` - Reverse container
- `find()` - Search for element
- `count()` - Count occurrences
- `max()` - Maximum of two values
- `min()` - Minimum of two values
- `swap()` - Swap two values
- `fill()` - Fill with value
- `copy()` - Copy elements
- `unique()` - Remove duplicates
- `binary_search()` - Binary search
- `lower_bound()` - Lower bound
- `upper_bound()` - Upper bound

---

## Memory Management

### `<memory>`
- `unique_ptr` - Unique pointer
- `shared_ptr` - Shared pointer
- `weak_ptr` - Weak pointer
- `make_unique()` - Create unique_ptr
- `make_shared()` - Create shared_ptr

### `<cstdlib>`
- `malloc()` - Allocate memory
- `free()` - Free memory
- `calloc()` - Allocate zero-initialized memory
- `realloc()` - Reallocate memory

---

## Utilities

### `<utility>`
- `pair` - Pair of values
- `make_pair()` - Create pair
- `swap()` - Swap values
- `move()` - Move semantics

### `<cstdlib>`
- `rand()` - Random number
- `srand()` - Initialize generator
- `atoi()` - String to int conversion
- `atof()` - String to float conversion
- `exit()` - Exit program
- `system()` - Execute system command

### `<ctime>`
- `time()` - Current time
- `clock()` - Processor clock
- `difftime()` - Time difference

---

## Exception Handling

### `<exception>`
- `exception` - Base exception class
- `what()` - Error message
- `throw` - Throw exception
- `try` - Try block
- `catch` - Catch exception

### `<stdexcept>`
- `runtime_error` - Runtime error
- `logic_error` - Logic error
- `out_of_range` - Out of range
- `invalid_argument` - Invalid argument

---

## Types and Limits

### `<limits>`
- `numeric_limits` - Numeric type limits

### `<climits>`
Constants such as:
- `INT_MAX` - Maximum int value
- `INT_MIN` - Minimum int value
- `CHAR_MAX` - Maximum char value

### `<cfloat>`
Constants such as:
- `FLT_MAX` - Maximum float value
- `DBL_MAX` - Maximum double value

---

## Notes

This list contains the most commonly used functions and classes from the C++ Standard Library. For complete documentation, visit [cppreference.com](https://en.cppreference.com/).
