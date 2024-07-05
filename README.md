# SLR Parser and Lexer

This project implements a Simple LR (SLR) parser and a lexical analyzer (lexer) for a simple C-like language. The parser and lexer are implemented in C++.

## Description

The project consists of two main components:
1. **SLR Parser (`slr_parser2.cpp`)**: Implements a bottom-up parser for the given grammar.
2. **Lexer (`lexer3.cpp`)**: Implements a lexical analyzer to scan the stream of characters from a program written in the above language and generate a stream of tokens.

## Features

- **Data Types**: Supports character (`char`) and integer (`int`). A string is implemented as an array of characters.
- **Assignments**: Variable assignments can be performed at the time of declaration.
- **Conditional Constructs**: Supports `if` and `else` statements. No `if` statement can exist without a corresponding `else`.
- **Loop Constructs**: Supports `for` loops.
- **Operators**: Supports relational operators (`>`, `<`, `=`) and arithmetic operators (`+`, `-`, `*`, `^`).
- **Function**: Only the `main()` function is supported.
- **Inline Functions**: Methods defined inside the class are treated as inline by default.
- **Input Files**: The program accepts two input files as command-line arguments.

## Usage

### Compilation

To compile the project, use the following command:

```sh
g++ slr_parser2.cpp lexer3.cpp -o parser

## Running the Program

To run the compiled program, provide two input files as command-line arguments:

```sh
./parser input1.txt input2.txt
