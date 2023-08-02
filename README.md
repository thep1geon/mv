# Mirtual Vachine (mv)
A *very* simple virtual machine written in C. Inspired by Tsoding.

[Writing VM for my Lisp in C — Day 1](https://www.youtube.com/watch?v=0irYsCYuZws&list=PLpM-Dvs8t0VY73ytTCQqgvgCWttV3m8LM) 
[Tsoding](https://www.youtube.com/@TsodingDaily)

# Quick Start
'
$ ./build <example program>
'
----------
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

    mov
        if only one paramter is provided, the value at that register (zero based) is pushed to that stack
        
        if two parameters are provided, the second value is put into the register of the first parameter

    pop
        if a paramter is provided, the top of the stack is popped into that register
        
        if no paramter is passed, the top is just popped

    jmp (and all the variants)
        '
        push 0
        push 1
        jmp 0
        ' 

        all jump instructions take the jump-point as the first parameter

        the other jump options take what you're comparing the top of the stack with and only jumping if that is true

    dump
        prints the stack
