# Python-like Interpreter in C++

A modern, extensible interpreter for a Python-inspired language, written from scratch in C++.

---

## Overview

This project is a robust, educational interpreter for a Python-like language. It supports indentation-based blocks, variables, arithmetic, loops, conditionals, function calls, string handling, and more. The codebase is modular and designed for both learning and extension.

---

## Features

- Python-like syntax: indentation-based blocks, familiar keywords, and operators
- Variables and arithmetic: assignment, math, and string operations
- Control flow: `if`, `else`, `while`, and `for` (with `range`)
- Function calls: built-in `print`, `range`, and extensible for more
- String literals: with escape sequences (`\n`, `\t`, `\\`, etc.)
- Comments: `#` to end of line
- Error handling: clear runtime and syntax errors
- Performance metrics: see compilation and interpretation time with `--timing`
- Modular, extensible C++ design

---

## Installation

1. **Clone the repository:**
   ```sh
   git clone https://github.com/yourusername/interpreter.git
   cd interpreter
   ```
2. **Build with CMake:**
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

---

## Usage

1. **Write your program** in a `.grm` file (see `examples/showcase.grm`).
2. **Run the interpreter:**
   ```sh
   ./RpnParser <filename> [--timing]
   ```
   - `<filename>`: Path to your `.grm` source file (required)
   - `--timing`: (Optional) Print compilation and interpretation time

   **Example:**
   ```sh
   ./RpnParser examples/showcase.grm --timing
   ```

---

## Example Code

```python
# Showcase: All supported features of this Python-like interpreter

print("========== Hello, world! ==========")
print("Hello, world!\n")

print("========== String Concatenation ==========")
name = "Alex"
print("Hello, " + name + "!\n")

print("========== String Escape Sequences ==========")
print("This is a string with a newline:\nAnd a tab:\tDone.")
print("But we can also escape symbols. For example: \\n \\t \\r\n")

print("========== Variable Assignment and Arithmetic ==========")
a = 5
b = 10
sum = a + b
print("Sum of", a, "and", b, "is", sum, "\n")

print("========== If-Else Statement ==========")
if sum > 10:
    print("Sum is greater than 10\n")
else:
    print("Sum is not greater than 10\n")

print("========== While Loop ==========")
counter = 0
while counter < 3:
    print("While loop iteration:", counter)
    counter = counter + 1
print("------------------------------\n")

print("========== For Loop with range(stop) ==========")
for i in range(5):
    print("i =", i)
print("------------------------------\n")

print("========== For Loop with range(start, stop) ==========")
for j in range(2, 6):
    print("j =", j)
print("------------------------------\n")

print("========== For Loop with range(start, stop, step) ==========")
for k in range(10, 0, -2):
    print("k =", k)
print("------------------------------\n")

print("========== Nested Loops and Arithmetic ==========")
print("Multiplication table (1 to 3):")
for x in range(1, 4):
    for y in range(1, 4):
        print(x, "*", y, "=", x * y)
    print("-----")
print("------------------------------\n")

# Comments are supported!
# The next line prints a final message
print("========== END OF SHOWCASE ==========")
print("Showcase complete!\n")
```

---

## Interpreter Potential

- **Educational tool**: Learn about interpreters, parsing, and language design.
- **Extensible**: Add user-defined functions, more data types, or even a REPL.
- **Performance**: Use as a base for experimenting with bytecode VMs or JIT compilation.
- **Integration**: Embed as a scripting engine in C++ projects.
- **Clear architecture**: Modular design with separate tokenizer, parser, AST, and interpreter components.

---

## Project Structure

```
interpreter/
├── include/         # Header files
├── src/             # Source files
├── examples/        # Example programs
├── tests/           # (Optional) Unit tests
├── CMakeLists.txt   # Build configuration
├── README.md        # This file
└── LICENSE
```
