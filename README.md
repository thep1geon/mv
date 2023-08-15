# Introduction
A simple virtual machine and programming language written in C. Inspired by Tsoding.

[Writing VM for my Lisp in C — Day 1](https://www.youtube.com/watch?v=0irYsCYuZws&list=PLpM-Dvs8t0VY73ytTCQqgvgCWttV3m8LM) 

[Tsoding](https://www.youtube.com/@TsodingDaily)

## Features
 - [X] Comments
 - [X] labels
 - [X] Dot Operator
 - [X] Memory
 - [X] Strings
 - [X] Built-in functions (e.g., "print_str")
 - [X] Utility instructions (e.g., "stack_size", "swap")
 - [X] Functions
 - [X] Including from other files

## Project Goals

The primary goal of this project is to create a functional virtual machine and programming language while learning and experimenting with language design, virtual machine architecture, and low-level programming concepts.

## Getting Started

Dependencies

- Make
- gcc

For Debian like distros: ```sudo apt-get install make gcc```

1. Clone the repo: ```git clone https://www.github.com/TheP1geon/mv```
2. Change the INLCUDE_PATH in main.c to the full path of the include folder of the repo
3. Compile with the makefile ```make```
4. [Write](#writing-your-own-programs) your own program or use one of the examples
5. Run: ```./mv <program> [-debug]```

## Writing Your Own Programs

1. Make a new file ending in ".mv"
2. Write the program (see the [Docs](https://github.com/TheP1geon/mv/blob/main/docs.md))
3. Save the file
4. Run the program with the compiled vm

## Examples

```mv
include "std.mv" ;; Inlcude the standard library which only has one function

str "Hello World!"
dupe 0 ;; Duplicate the pointer
pop 0 ;; Pop it into register 0
add ;; add the pointer to the length of the string
mem_write . 10 ;; Write at that address, 10. A newline character 
inc 0 ;; Increment the pointer
mem_write . 0 ;; Write a 0
pop ;; pop the pointer

call print_str ;; Expects the pointer in register 0
```

Prints "Hello World!" with a newline to the standard output

```
Hello World!
```

## Contributing

Feel free to contribute to the project. Be sure to:

- Clearly describe the changes made
- Test the changes to ensure they don't introduce new issues
- Follow the project's coding style and guidelines

## Quick Start

Run:
```bash
./test.py
```
To see all the examples and their output
