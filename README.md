# Nuva Programming Language

Nuva — a small, expressive language for learning how languages are made.

## What is Nuva?
Nuva is a dynamically-typed, interpreted language built from scratch in C. It supports variables, arithmetic and logical operations, string manipulation, robust control flow (including a unique `repeat N times` loop), functions with recursion, and basic I/O capabilities. It was created as an educational exercise in programming language implementation.

## How it was built
The implementation pipeline is standard and modular:
1. **Lexical Analysis**: Flex (`lexer.l`) reads the raw source `.nv` files and tokenizes the text into meaningful symbols (keywords, identifiers, numbers, strings).
2. **Parsing & AST**: Bison (`parser.y`) processes the token stream using a formal grammar to construct an Abstract Syntax Tree (AST). The AST represents the structure and order of operations.
3. **Interpretation**: A tree-walking interpreter in C (`interpreter.c`) traverses the AST directly. It uses an environment stack to manage scope and variables, evaluating expressions and executing statements on the fly.

## Design Decisions
- **Dynamic Typing**: Types are checked at runtime at the value level rather than compile-time. This simplifies the syntax (no need to specify types like `int` or `float` when declaring variables) and aligns with modern scripting language ergonomics.
- **Tree-walking interpreter**: Instead of compiling to a bytecode/VM format, the C backend walks the AST nodes directly. While a VM is typically faster, a tree-walking interpreter is significantly easier to build, debug, and understand for educational purposes, providing a clean separation between parsing and execution.

## Challenges and Solutions
- **Dangling-Else Ambiguity**: The classic `if-else` shift/reduce conflict was resolved by using Bison's `%nonassoc` precedence. By declaring `%nonassoc LOWER_THAN_ELSE` and `%nonassoc TOKEN_ELSE` (where `ELSE` has higher precedence), Bison prefers shifting the `else` token rather than reducing an unmatched `if`, correctly binding `else` to the nearest `if`.
- **String and Number Auto-coercion**: Combining strings and numbers (e.g., `"Value: " + 10`) required careful type inspection in the interpreter's binary operator logic. When `+` encounters a string on either side, it coerces both operands to strings dynamically and allocates a concatenated result.
- **Scoping with Environment Stack**: Properly shadowing variables in blocks and functions required implementing a linked-list-based environment stack. When entering a block `{ }` or a function, a new environment is pushed; when exiting, it is popped.

## How to Build and Run
Make sure you have GCC, Flex, and Bison installed.
1. Run `make` to compile the interpreter.
2. Run `./nuva examples/10_all_features_demo.nv` to see the language in action!

For full details on syntax and usage, see `Language_Manual.txt`.

## License
MIT License
