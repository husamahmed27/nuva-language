# Nuva

> A custom, dynamically-typed programming language written in C using Flex and Bison.

## Description
Nuva is a small, expressive, and dynamically-typed programming language built from scratch. It was designed as an educational language to demonstrate compiler and interpreter construction. It features a tree-walking interpreter, variables, dynamic type coercion, arithmetic operations, robust control flow, and basic I/O capabilities.

## Features
- **Dynamic Typing:** No need to specify types like `int` or `float`. Types are checked and managed dynamically at runtime.
- **Auto-coercion:** Seamlessly combine strings and numbers (e.g. `"Value: " + 10`).
- **Flexible Control Flow:** Standard `if-else` branching and a unique `repeat N times` looping construct.
- **First-class Scoping:** Block-level and function-level variable scoping powered by an environment stack.
- **Tree-Walking Interpreter:** A clear and accessible C implementation that traverses an Abstract Syntax Tree (AST) directly.

## Prerequisites
To build Nuva from source, you will need the following tools installed on your system:
- **GCC** (GNU Compiler Collection)
- **Flex** (Fast Lexical Analyzer)
- **Bison** (GNU Parser Generator)
- **Make** (Build automation tool)

## Installation
Nuva includes a Makefile for easy compilation. 

1. **Clone the repository:**
   ```bash
   git clone https://github.com/husamahmed27/nuva-language.git
   cd nuva-language
   ```

2. **Build the project:**
   ```bash
   make
   ```
   This will generate the lexer and parser, compile the C source files, and produce the `nuva` executable.

## Usage: Hello World
Create a new file named `hello.nv` and add the following Nuva code:

```nuva
// hello.nv
print("Hello, World!");
```

Then, execute the file using the Nuva interpreter:

```bash
./nuva hello.nv
```

**Output:**
```
Hello, World!
```

## License
MIT License
