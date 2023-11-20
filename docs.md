# Documentation

Welcome to the offical documentation of the virtual machine and the programming
language 'Vassembly'

## Table of Contents

- [Introduction](#introduction)
- [Language Basics](#language-basics)
    - [Stack Operations](#stack-operations)
    - [Arithmetic Operations](#arithmetic-operations)
    - [Jumps](#jumps)
    - [Labels & Functions](#labels-&-functions)
    - [Memory Operations](#memory-operations)
    - [Utility Operations](#utility-operations)
    - [Screen Stuff](#screen-stuff)
    - [Other](#other)
- [Standard Library](#standard-library)
- [Virtual Machine](#virtual-machine)
    - [Stack](#stack)
    - [Registers](#registers)
    - [Heap Memory](#heap)
    - [Programs](#programs)
    - [Label Table](#label-table)
- [Final Notes](#final-notes)

## Introduction

The Vassembly language is the assembly of the virtual machine. Both the virtual
machine and the language are written in C; Therefore the language is also interpreted
in C

## Language Basics

An example program might look something like this:
```mv
push 19
push 23
add

dump
```

The numbers 19 and 23 are pushed to the stack, added, and then printed

```
42
```

### Stack Operations

- push: Pushes the parameter onto the stack 

- push_lit "string": Pushes, one character at a time, onto the stack

- pop: removes the top of the stack 

- pop [index]: removes the top of stack and places into register index [index] ([Registers](#registers))

- dupe: Duplicates the top of the stack

- dupe [index]: If a index is provided it duplicates at that index where 0 is top of the stack

- swap: Swaps the top element with the second-to-top element

### Arithmetic Operations

- add: Adds the first two elements on the stack and pushes the result

- sub: Subtracts the top element from the second element and pushes the result

- mult: Multiples the two elements and pushes the result

- div: Divides the top element by the second element and pushes the result

- mod: Divides the top element by the second element and pushes the remainder

- inc: Increments the top of the stack

- inc [index]: Increments the stack at [index] (where 0 is the top of the stack) 

- dec: Decrements the top of the stack

- dec [index]: Decrements the stack at [index] (where 0 is the top of the stack) 

### Labels & Functions

#### Note:
During the execution of a program when a label is encountered, it is run like normal;
However, when a function is found, it is skipped.

- [label:] : Creates a new label that you can jump to

- func [label:] : Creates a new function, which is just a fancy label

- ret: All functions need to end in ret which returns from the function

- call [label]: Makes a call to the function [label]

### Jumps

- jmp [label]: Unconditional jump to [label]

- jmp_gt [label] [condition]: Jumps to [label] if the top of the stack is greater than [condition]

- jmp_gteq [label] [condition]: Jumps to [label] if the top of the stack is greater than or equal to [condition]

- jmp_lt [label] [condition]: Jumps to [label] if the top of the stack is less than [condition]

- jmp_lteq [label] [condition]: Jumps to [label] if the top of the stack is less than or equal to [condition]

- jmp_eq [label] [condition]: Jumps to [label] if the top of the stack is to [condition]

- jmp_neq [label] [condition]: Jumps to [label] if the top of the stack is not to [condition]

- cmp: compares the top of the stack to the second item on the stack

| Operation   | Result    |
|--------------- | --------------- |
| !=   |  0  |
| ==   |  1  |
| <   |  2  |
| <=   |  3  |
| >   |  4  |
| >=   |  5  |


### Memory Operations

- mem_read [address]: Pushes the value at [address] onto the stack

- mem_write [address] [value]: Sets the value at [address] to [value]

- str "string": Finds the next available address in memory and writes the string there.
str, in this order, pushes the length of the string and then the pointer to start of the string

- arr [size]: allocates [size] on the heap and pushes the pointer to the start
on to the stack

- arr [size] [default]: allocates [size] on the heap and pushes the pointer to the start
on to the stack. The array is filled with the [default]

### Screen Stuff

The screen has its own video buffer. The two memory operations change the video buffer.

The video buffer is an array of integers but is printed as ascii characters

- vid_mem_read [address]: Pushes the value at [address] onto the stack

- vid_mem_write [address] [value]: Sets the value at [address] to [value]

- draw: Renders the video buffer to the standard output

- clear: clears the video buffer; sets everything to 0

### Utility Operations

- dump: Prints the stack as numbers

- dump [index]: Prints, as a number, the index in the stack (0 being the top of the stack)

- print: Prints the stack as characters

- print [index]: Prints, as a character, the index in the stack (0 being the top of the stack)

- size: Pushes to the top of the stack the size of the stack

- stop: Stops the program right there (The program stops with an error)

### Other

#### Note
These operators are related to the Virtual Machine's registers [Registers](#registers)

- mov [index]: If only a register [index] is provided, the value at that register is pushed to the stack

- mov [index] [value]: [value] is placed in register [index]

---

- Dot Operator: Some operators' parameters be replaced with a dot ('.')

In order to achieve some level of safety, only these operators can use the dot operator:
- push
- mem_read
- mem_write
- mov

## Standard Library

To use the standard library you need to modify the INLCUDE_PATH in main.c to 
where ever you cloned the repo

## Virtual Machine

The virtual machine is [Stack](#stack) based

### Stack

I programmed the stack from stratch using vanilla C. It's got some operators 
listed here: [Stack Operations](#stack-operations)

### Registers

The machine has 10 registers, this is subject to change, that the language can 
interact with and use. The registers is just an array of numbers

### Heap

The machine also has a much larger array of numbers for the heap memory. I'll 
be completely honest, I don't know if heap is the right term. The language can
also interact with the heap using the [Memory Operations](#memory-operations)

### Programs

The program struct is just an array of Inst's (another struct), it's size and 
the file that the program came from 

### Label Table

The Virtual Machine keeps track of labels and functions using a label table and 
***very*** simple hashing algorithm


# Final Notes

This is the first time I've ever something this big. I only just started learning
how to code last year. I did this to learn more about computers and how they 
work.
