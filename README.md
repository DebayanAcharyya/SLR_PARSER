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
- **Input Files**: The program accepts two input files as command-line arguments.

## Usage

### Compilation

To compile the project, use the following command:

```sh
g++ slr_parser2.cpp lexer3.cpp -o parser
```
### Running the Program

To run the compiled program, provide two input files as command-line arguments:

```sh
./parser <grammar.txt> <input_file.txt>
```
### Command Line Arguments
The program expects the following command-line arguments:

- **grammar.txt**: This file should contain the CFG for the parser
- **input_file.txt**: The input file containing the program to be parsed.

## File Descriptions

### `lexer3.cpp`

Contains the implementation of the lexical analyzer (lexer), which includes:

- Token generation
- Symbol table maintenance
- Character and integer literal recognition
- Keyword and identifier recognition

### `slr_parser2.cpp`

Contains the implementation of the SLR parser, which includes:

- Parsing table construction
- FIRST AND FOLLOW set computation
- Item-set construction
- Bottom-up parsing algorithm (Parsing Table and Parsing process included)

### `finalgrammar.txt`
Contains the CFG for this parser

### `lexer_input.txt`
Contains the program to be parsed

### `token.h`
Header file containing Token types and Token structure details




