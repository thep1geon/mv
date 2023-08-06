# Mirtual Vachine (mv)
A ***very*** simple virtual machine written in C. Inspired by Tsoding.

[Writing VM for my Lisp in C — Day 1](https://www.youtube.com/watch?v=0irYsCYuZws&list=PLpM-Dvs8t0VY73ytTCQqgvgCWttV3m8LM) 

[Tsoding](https://www.youtube.com/@TsodingDaily)

# Quick Start
```console
$ ./mv <example program>
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

## abc.mv
```
push A

loop:
dupe 0
push 32 ; Get the lower case version
add

jmp_eq end z ; jump to the end if the top of the stack is z

push 32 ; pushes a space

push 32
dupe 2 ; Find the next letter
sub

inc 0

jmp_lteq loop Z ; jump back to the beginning of the loop

end:
print ; print the stack in ascii form
```
Output:
```
--------------
Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj Kk Ll Mm Nn Oo Pp Qq Rr Ss Tt Uu Vv Ww Xx Yy Zz
--------------
```

## count-to-10.mv 
```
push 0 ; Push the initial 0 to the stack

loop:
dupe 0 ; duplicate the top of the stack
inc 0 ; increment the top of the stack

jmp_lt loop 10 ; jump to the begining of the loop if the top of the stack is less than 10
dump ; print the integer representation
```
Output:
```
--------------
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
--------------
```
## fact.mv
```
mov 0 5 ; push 5 into register 0

mov 0 ;
dec 0 ; Find the number of times to multiply
pop 9 ;

mov 0 ; move the value of reg. 0 onto the stack

loop:
dupe 0 ; push to the stack of every number til 0
dec 0
jmp_neq loop 0
pop

mov 9 ; reg. 9 will hold the "loop counter"

mult_loop:
pop 9
mult
mov 9
dec 0
jmp_neq mult_loop 0
pop ; pop the loop counter

dump ; print the numbers of the stack
```
Output:
```
--------------
120
--------------
```

## fib.mv
```
push 0
push 1
push 0

loop:
pop 9

dupe 1
dupe 1
add

mov 9
inc 0
jmp_lt loop 15
pop
dump
```
Output:
```
--------------
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
--------------
```

## hello-world.mv
```
push_lit Hello, world!
pop ; the new line character

print
```
Output:
```
--------------
Hello, world!
--------------
```

## labels.mv
```
push 10
push 10
add

jmp_eq equal 20

pop
push_lit Not Equal
jmp end

equal:
pop
push_lit Equal
jmp end

end:
pop ; pop the new line character
print
```
Output:
```
--------------
Equal
--------------
```

## squares.mv
```
mov 0 1
push 0

loop:
pop 9

mov 0
mov 0
mult
mov 0
inc 0
pop 0

mov 9
inc 0
jmp_lt loop 10

pop
dump
```
Output:
```
--------------
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
--------------
```
