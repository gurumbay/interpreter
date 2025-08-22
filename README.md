# Python-like Interpreter in C++

A modern, feature-rich interpreter for a Python-inspired language, written from scratch in C++. This project demonstrates advanced language implementation techniques including lexical analysis, parsing, AST construction, and interpretation.

---

## 🚀 Features Overview

### Core Language Features
- **Python-like syntax** with indentation-based blocks
- **Dynamic typing** with automatic type inference
- **Comprehensive error handling** with runtime and syntax error messages
- **Comments** using `#` to end of line
- **Performance metrics** with `--timing` flag for compilation and interpretation time

### Data Types
- **Numbers**: Floating-point arithmetic (integers and decimals)
- **Strings**: Full string support with escape sequences (`\n`, `\t`, `\\`, `\"`)
- **Lists**: Dynamic arrays with indexing and iteration
- **Ranges**: Efficient sequence generation for loops
- **Functions**: Both built-in and user-defined functions
- **Iterators**: Support for iterating over collections

### Operators & Expressions
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `**` (power)
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `and`, `or`, `not`
- **Unary**: `-` (negation), `not` (logical not)
- **Assignment**: `=` with proper operator precedence
- **Parentheses**: Full support for expression grouping

### Control Flow
- **Conditionals**: `if`, `else` with proper scoping
- **Loops**: `while` loops with `break` and `continue`
- **For loops**: `for` loops with `range()` and iterable objects
- **Nested control structures**: Full support for complex logic

### Functions & Scoping
- **User-defined functions**: `def` with parameters and return values
- **Built-in functions**: `print()`, `range()`, `len()`
- **Variable scoping**: Local, global, and nested scope support
- **Recursion**: Full support for recursive function calls
- **Closures**: Functions capture their lexical environment

### Collections & Iteration
- **List literals**: `[1, 2, 3, 4, 5]`
- **List indexing**: `list[0]`, `list[-1]` with bounds checking
- **String indexing**: `string[0]` for character access
- **Range objects**: `range(stop)`, `range(start, stop)`, `range(start, stop, step)`
- **Iteration**: `for` loops over lists, strings, and ranges

### String Operations
- **Concatenation**: `+` operator for string joining
- **Escape sequences**: `\n`, `\t`, `\\`, `\"`, `\r`
- **Length**: `len(string)` and `string.length` property
- **Indexing**: Character access with bounds checking

---

## 🛠️ Installation & Build

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.10 or higher

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/gurumbay/interpreter.git
cd interpreter

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run an example
# On Windows (MSVC):
./Debug/interpreter.exe ../examples/showcase.grm --timing
# On Unix-like systems:
./interpreter ../examples/showcase.grm --timing
```

---

## 📖 Usage

### Basic Usage
```bash
# On Windows (MSVC):
./Debug/interpreter.exe <filename> [--timing]

# On Unix-like systems:
./interpreter <filename> [--timing]
```

**Parameters:**
- `<filename>`: Path to your `.grm` source file (required)
- `--timing`: Optional flag to display compilation and interpretation time

### Example Programs
The `examples/` directory contains comprehensive test files:
- `showcase.grm` - Complete feature demonstration
- `functions_test.grm` - Function definitions and scoping
- `operations_priority_test.grm` - Operator precedence and associativity
- `fibonacci.grm` - Recursive algorithm example
- `nested_loops.grm` - Complex loop structures

---

## 💡 Language Examples

### Basic Syntax & Variables
```python
# Numbers and arithmetic
x = 42
y = 3.14
result = x * y + 10
print("Result:", result)

# Strings and concatenation
name = "World"
greeting = "Hello, " + name + "!"
print(greeting)
```

### Control Flow
```python
# Conditional statements
if x > 40:
    print("x is greater than 40")
else:
    print("x is 40 or less")

# While loops
counter = 0
while counter < 5:
    print("Counter:", counter)
    counter = counter + 1

# For loops with range
for i in range(3):
    print("Loop iteration:", i)

# Nested loops
for i in range(1, 4):
    for j in range(1, 4):
        print(i, "*", j, "=", i * j)
```

### Functions & Scoping
```python
# Function definition
def factorial(n):
    if n <= 1:
        return 1
    else:
        return n * factorial(n - 1)

# Function calls
result = factorial(5)
print("5! =", result)

# Nested functions with closures
def outer_function(x):
    def inner_function(y):
        return x + y
    return inner_function

add_five = outer_function(5)
print("5 + 3 =", add_five(3))
```

### Collections & Iteration
```python
# Lists
numbers = [1, 2, 3, 4, 5]
print("List:", numbers)
print("First element:", numbers[0])
print("List length:", len(numbers))

# String operations
text = "Python"
print("String:", text)
print("First character:", text[0])
print("String length:", len(text))

# Range with different parameters
for i in range(5):        # 0, 1, 2, 3, 4
    print(i)

for j in range(2, 6):     # 2, 3, 4, 5
    print(j)

for k in range(10, 0, -2): # 10, 8, 6, 4, 2
    print(k)
```

### Advanced Features
```python
# Operator precedence and associativity
result = 1 + 2 * 3**2 - 4 / 2
print("Complex expression:", result)

# Logical operators
a = 1 and 0 or 1
print("Logical result:", a)

# String escape sequences
print("Line 1\nLine 2\tTabbed")
print("Escaped quotes: \"Hello\"")
```

---

## 🏗️ Architecture

### Modular Design
The interpreter follows a clean, modular architecture:

```
interpreter/
├── include/
│   ├── core/           # Core interpreter components
│   │   ├── AST.h       # Abstract Syntax Tree definitions
│   │   ├── Parser.h    # Recursive descent parser
│   │   ├── Tokenizer.h # Lexical analyzer
│   │   ├── Interpreter.h # Tree-walking interpreter
│   │   └── Environment.h # Variable scope management
│   └── objects/        # Runtime object system
│       ├── Object.h    # Base object class
│       ├── NumberObject.h
│       ├── StringObject.h
│       ├── ListObject.h
│       ├── RangeObject.h
│       ├── FunctionObject.h
│       └── IteratorObject.h
├── src/                # Implementation files
├── examples/           # Example programs
└── CMakeLists.txt      # Build configuration
```

### Key Components
- **Tokenizer**: Converts source code into tokens
- **Parser**: Builds AST from token stream
- **Interpreter**: Executes AST nodes
- **Environment**: Manages variable scopes
- **Object System**: Runtime type system

---

## 🎯 Use Cases

### Educational
- **Language Design**: Learn about interpreter implementation
- **Compiler Theory**: Study lexical analysis and parsing
- **Runtime Systems**: Understand object systems and memory management
- **Software Architecture**: Explore modular design patterns

### Development
- **Scripting Engine**: Embed as a scripting language in C++ applications
- **Configuration Language**: Use for application configuration
- **Prototyping**: Rapidly prototype language features
- **Testing**: Test language design concepts

### Extension
- **New Data Types**: Add custom object types
- **Built-in Functions**: Extend the standard library
- **Optimizations**: Implement bytecode compilation or JIT
- **Language Features**: Add classes, modules, or other constructs

---

## 🔧 Development

### Adding New Features
The modular design makes it easy to extend:

1. **New Object Types**: Inherit from `Object` base class
2. **Built-in Functions**: Add to `setupBuiltinFunctions()` in Interpreter
3. **Syntax Extensions**: Extend Parser and AST classes
4. **Optimizations**: Modify Interpreter or add compilation passes

### Testing
Run the example programs to verify functionality:
```bash
# On Windows (MSVC):
./Debug/interpreter.exe examples/showcase.grm
./Debug/interpreter.exe examples/functions_test.grm
./Debug/interpreter.exe examples/operations_priority_test.grm

# On Unix-like systems:
./interpreter examples/showcase.grm
./interpreter examples/functions_test.grm
./interpreter examples/operations_priority_test.grm
```

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for:
- Bug fixes
- New language features
- Performance improvements
- Documentation enhancements
- Example programs

---

*Built with ❤️ in C++ - A comprehensive Python-like interpreter for learning and experimentation.*
