# Mirtual Vachine (mv)
A *very* simple virtual machine written in C. Inspired by Tsoding.

[Writing VM for my Lisp in C — Day 1](https://www.youtube.com/watch?v=0irYsCYuZws&list=PLpM-Dvs8t0VY73ytTCQqgvgCWttV3m8LM) 

[Tsoding](https://www.youtube.com/@TsodingDaily)

# Quick Start
```console
$ ./run <example program>
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

    mov
        if only one paramter is provided, the value at that register (zero based) is pushed to that stack
        
        if two parameters are provided, the second value is put into the register of the first parameter

    pop
        if a paramter is provided, the top of the stack is popped into that register
        
        if no paramter is passed, the top is just popped

    jmp (and all the variants)
        push 0
        push 1
        jmp 0

        all jump instructions take the jump-point as the first parameter

        the other jump options take what you're comparing the top of the stack with and only jumping if that is true

    dump
        prints the stack as numbers
    
    print
        prints the ascii representation of the stack

# Examples:
## count-to-10
Prints the first 10 natural numbers

```
push 0

dupe 0
inc 0

jmp_neq 1 10
dump
```
--------------
Output:
```
--------------
10
9
8
7
6
5
4
3
2
1
0
--------------
```

## fact
Calculates the factorial of the number in register 0

```
mov 0 5

mov 0
dec 0
pop 9

mov 0

dupe 0
dec 0
jmp_neq 5 0
pop

mov 9

pop 9
mult
mov 9
dec 0
jmp_neq 10 0
pop

dump
```
--------------
Output:
```
--------------
120
--------------
```

## fib
Calculates the first 15 fibonacci sequence

``` Code  
push 0
push 1
push 0

pop 9

dupe 1
dupe 1
add

mov 9
inc 0
jmp_lt 3 15
pop
dump
```
--------------
Output:
```
--------------
987
610
377
233
144
89
55
34
21
13
8
5
3
2
1
1
0
--------------
```

## hello world
Prints hello world

``` 
push H
push e
push l 
push l
push o
push 32
push w
push o
push r
push l
push d
print
```
--------------
Output:
```
--------------
Hello world
--------------
```

## squares
```
mov 0 1
push 0

pop 9

mov 0
mov 0
mult
mov 0
inc 0
pop 0

mov 9
inc 0
jmp_lt 2 10

pop
dump
```
--------------
``` 
Output:
--------------
100
81
64
49
36
25
16
9
4
1
--------------
```

# NOTE:
comments are not natively supported
