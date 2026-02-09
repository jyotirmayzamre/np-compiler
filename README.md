# NP Compiler

A compiler implementation for the NanoPascal subset of the Pascal programming language, featuring lexical analysis, parsing, intermediate code generation (three-address code), and x86-64 assembly code generation. This project was completed as Programming Assignment 4 for CS-3340: Programming Languages and Translation at Ashoka University.

## Overview

This project implements a complete compiler pipeline that translates NP source code into executable x86-64 assembly code. The compiler follows the System V ABI calling convention and generates optimized assembly output.

## Features

- **Lexical Analysis**: Token recognition and classification
- **Syntax Analysis**: Context-free grammar parsing with semantic actions
- **Intermediate Code Generation**: Three-address code (quadruples) representation
- **Target Code Generation**: x86-64 assembly code generation
- **Symbol Table Management**: Multi-level symbol tables for scope management

## Architecture

### Components

1. **Lexer**: Tokenizes the input source code
2. **Parser**: Builds an abstract syntax tree and performs semantic analysis
3. **Quad Generator**: Produces intermediate three-address code
4. **Assembly Emitter**: Generates x86-64 assembly code from quadruples

### Memory Management

- **Global Data**: Stored in the `.data` section with 8-byte alignment
- **Local Variables**: Allocated on the stack with negative offsets from `%rbp`
- **Parameters**: First 6 parameters in registers (`%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`), remaining on stack
- **Return Values**: Stored as local variables at negative offset from `%rbp`


## Requirements

- GCC or compatible C++ compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make

## Installation

Clone the repository:

```bash
git clone https://github.com/jyotirmayzamre/np-compiler.git
cd np-compiler
```

## Building the Compiler

To compile the compiler from source:

```bash
make
```

This command will:
- Generate the lexer from `lexer.l`
- Generate the parser from `parser.y`
- Compile all source files
- Link the final compiler executable

## Usage

### Running Test Cases

To compile all test cases and generate both quadruples and assembly code:

```bash
mkdir Quads
mkdir Asm
make test
```

This will process all files in the `Tests/` directory and output:
- Quadruple intermediate code in the `Quads/` directory
- x86-64 assembly code in the `Asm/` directory

### Compiling Individual Programs

To compile a specific NP source file, use the same naming convention as the files in the `Tests/` directory and place the source file in `Tests/`.

### Cleaning Build Files

To remove all generated files and clean the project:

```bash
make clean
```

This removes:
- Compiled object files
- Generated lexer and parser files
- Output quadruple files
- Output assembly files
- The compiler executable

## Code Generation Details

### Assembly Structure

Generated assembly files follow this structure:

1. **Data Segment** (`.data`):
   - Global variables declared with `.quad`
   - String literals declared with `.asciz`
   - Constants with initial values

2. **Text Segment** (`.text`):
   - Function prologues (stack frame setup)
   - Function bodies (quad-to-assembly translation)
   - Function epilogues (stack frame teardown)

### Supported Operations

- **Arithmetic**: Addition, subtraction, multiplication, division
- **Assignments**: Variable assignments and constant propagation
- **Conditionals**: Comparison operations with conditional jumps
- **Function Calls**: Parameter passing and return value handling
- **Control Flow**: Unconditional and conditional jumps

## Test Cases

The project includes five test cases demonstrating:
- Basic arithmetic operations
- Variable declarations and assignments
- Function definitions and calls
- Control flow structures
- Expression evaluation

All test cases have been validated to generate correct assembly code.

## Known Limitations

- Functions with more than 6 parameters may require additional validation for stack parameter handling
- The quad separation process iterates through the global quad array, which could be optimized

## Implementation Notes

### Symbol Table

The symbol table implementation includes:
- Multi-level scope management
- Support for global and local variables
- Constant and literal tracking with `initValue` and `isLiteral` fields
- Type information storage

### Quad Separation

Quadruples are initially generated in a global array and then separated by function using the `separateQuads()` function. This process:
- Organizes quads by function scope
- Remaps jump destinations to local quad indices
- Enables proper function prologue/epilogue generation

