Cristian McGee Singular Developer on PL/0 Compiler and Virtual Machine

This repository contains an implementation of a compiler for the PL/0 programming language. PL/0 is a minimalist educational language, often used to teach the basics of compiler construction. The language supports constants, variables, and procedures, with a simple syntax and structure, making it ideal for demonstrating key compilation techniques.

Compiler Overview
The PL/0 compiler in this project follows a standard compilation process, including:

Lexical Analysis: Converts the source code into tokens.
Syntax Analysis: Uses a recursive descent parser to build a syntax tree from the token stream.
Semantic Analysis: Ensures the correctness of operations (e.g., type checking, scope management).
Code Generation: Produces intermediate assembly-like code that can be executed by a virtual machine or further optimized.
Error Handling: Detects and reports both syntactic and semantic errors in the source code.

How to use:

The program runs where you feed it PL/0 code that follows the grammatical correctness of the EBNF given by the professor.

If your PL/0 has an error the program stops compiling and the program prints the error.

If the program is accurate and well it simply prints the PL/0 and shows it in Assembly Language. This gives an ELF file that can be used in the virtual machine (vm.c).

how to run the PL/0 compiler:

open your directory to the file

run:
gcc -o compiler compiler.c

followed with:
./compiler input.txt

additionally you follow with

./compiler input.txt output.txt

Once you have your elf.txt you can use it on vm.c.

gcc -o vm vm.c

followed with either:
./vm elf.txt

or

./vm elf.txt output.txt
