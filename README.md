# AlxLang*

## Yet Another General Purpose Programming Language No One Asked For

##### (YAGPPLNOAF)

AlxLang* is a C++-like programming language that compiles to assembly.

It is not meant to be used in production, but rather as a learning experience in compiler design
me and anyone else who wants to contribute.

\* Name is a work in progress. 

### How it works

Front-end:

* User input is lexed into tokens.
* Tokens are parsed into an AST.

Back-end:

* AST is converted into an IR.
* IR is lowered. (TODO)
* Lowered IR is converted to MIR. (WIP)
* MIR is converted to assembly which is written to a file. (TODO)
* NASM is used to assemble the assembly file into an object file.
* ld is used to link the object file into an executable.

Currently, the back-end only supports x86-64 assembly. 

I aim for the back-end to be as machine and language agnostic as possible, so in theory
it should be possible to add support for other architectures and languages.

#### Dependencies

* CMake
* NASM
* ld
* g++
* C++20