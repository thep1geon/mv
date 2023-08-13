# Mirtual Vachine (mv)
A ***very*** simple virtual machine written in C. Inspired by Tsoding.

[Writing VM for my Lisp in C — Day 1](https://www.youtube.com/watch?v=0irYsCYuZws&list=PLpM-Dvs8t0VY73ytTCQqgvgCWttV3m8LM) 

[Tsoding](https://www.youtube.com/@TsodingDaily)

# Quick Start
```console
$ ./mv <example program> [-debug]
```
# Instructions

    add, sub, div, mod, mult 
        Pops the top two numbers off the stack and pushes the result

    inc, dec
        increments, or decrements, the stack at that index
        
        if no index is provided the top of the stack is updated
    
    dupe
        pushes the value at the index provided

    push
        pretty self-explanatory

    push_lit
        pushes the literal (ascii) everything after the instruction

    mov
        if only one paramter is provided, the value at that register (zero based) is pushed to that stack
        
        if two parameters are provided, the second value is put into the register of the first parameter

    pop
        if a paramter is provided, the top of the stack is popped into that register
        
        if no paramter is passed, the top is just popped

    jmp (and all the variants)
        all jump instructions take the jump-point as the first parameter

        the other jump options take what you're comparing the top of the stack with and only jumping if that is true

    dump
        prints the stack as numbers
    
    print
        prints the ascii representation of the stack

# Examples:
labels.mv
Output: 
Equal
----------------------------------------------------

fact.mv
Output: 
120
----------------------------------------------------

squares.mv
Output: 
1
4
9
16
25
36
49
64
81
100
----------------------------------------------------

dot-operator.mv
Output: 
59
59
59
----------------------------------------------------

reg.mv
Output: 
0
----------------------------------------------------

str.mv
Output: 
Jello World!
Another Jello World!
----------------------------------------------------

fib.mv
Output: 
0
1
1
2
3
5
8
13
21
34
55
89
144
233
377
610
987
----------------------------------------------------

abc.mv
Output: 
Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj Kk Ll Mm Nn Oo Pp Qq Rr Ss Tt Uu Vv Ww Xx Yy Zz
----------------------------------------------------

functions.mv
Output: 
This is a Function!
----------------------------------------------------

hello-world.mv
Output: 
Hello, World!
----------------------------------------------------

include.mv
Output: 
ABCD
----------------------------------------------------

count-to-10.mv
Output: 
0
1
2
3
4
5
6
7
8
9
10
----------------------------------------------------
