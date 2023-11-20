#ifndef __STACK_H
#define __STACK_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "etypes.h"
#include "err.h"

#ifndef STACK_CAP
#define STACK_CAP 1024
#endif

// boolean stuff
typedef int bool;
#define true 1
#define false 0

// Node stuff

// Don't even know why I did this but I really don't want to change it
typedef struct {
    long data;
} Node;

// Node related functions
Node* node_alloc(long data);
void print_node(Node n);
void print_node_ascii(Node n);

Node* node_alloc(long data) {
    Node *node = (Node *)(malloc(sizeof(Node)));

    if (node == NULL) {
        fatal_err(new_error(STACK_NodeAllocFail, __LINE__, __FILE__));
    }

    node->data = data;

    return node;
}

inline void print_node(Node n) { printf("%ld\n", n.data); }
inline void print_node_ascii(Node n) { 
    printf("%c", (char)n.data); 
}

//Stack stuff

typedef struct {
    Node* data[STACK_CAP];
    size_t size;
} Stack;

// Stack functions
Stack* new_stack();
void print(Stack *stack);
void print_ascii(Stack *stack);
void push(Stack* s, long data);
void push_node(Stack* s, Node* n);
Node pop(Stack* s);
Node* peek(Stack* s);
bool is_empty(Stack* s);
int search(Stack *s, long data);
Node get(Stack *s, size_t index);
void rev(Stack *s);
void clear(Stack *s);
void copy(Stack *dest, Stack *src);

Stack* new_stack() {
    Stack* s = (Stack*)(malloc(sizeof(Stack)));

    if (!s) {
        fatal_err(new_error(STACK_StackAllocFail, 
                      __LINE__, __FILE__));
    }

    s->size = 0;
    return s;
}

void print(Stack* stack) {
    if (stack->size == 0) {
        fprintf(stdout, "Empty Stack\n");
        return;
    }

    for (int i = 0; i < (int)stack->size; i++) {
        print_node(*stack->data[i]);
    }
}

void print_ascii(Stack* stack) {
    if (stack->size == 0) {
        fprintf(stdout, "Empty Stack\n");
        return;
    }

    for (int i = 0; i < (int)stack->size; i++) {
        print_node_ascii(*stack->data[i]);
    }
    printf("\n");
}

//    <- push
// 81
// 23
// 42

void push(Stack* s, long data) {
    if (s->size == STACK_CAP) {
        fatal_err(new_error(STACK_StackOverflow,
                      __LINE__, __FILE__));
    }

    s->data[s->size++] = node_alloc(data);
}

void push_node(Stack* s, Node* n) {
    if (s->size == STACK_CAP) {
        fatal_err(new_error(STACK_StackOverflow,
                      __LINE__, __FILE__));
    }

    s->data[s->size] = n;
    s->size++;
}

// 81 <- * pop *
// 23
// 42

Node pop(Stack* s) {
    if (s->size == 0) {
        fatal_err(new_error(STACK_EmptyStackPop,
                      __LINE__, __FILE__));
    }

    Node popped_node = *s->data[--s->size];
    // Free the memory and return the value of the Node, not the reference
    free(s->data[s->size]);
    return popped_node;
}

// Peek can return a Node pointer
Node* peek(Stack* s) {
    if (s->size == 0) {
        non_fatal_err(new_error(STACK_EmptyStackPeek, 
                                  __LINE__, __FILE__));
        return NULL;
    }

    return s->data[s->size - 1];
}

inline bool is_empty(Stack* s) { return s->size == 0; }

int search(Stack* s, long data) {
    for (size_t i = 0; i < s->size; ++i) {
        Node n = *s->data[i];
        if (n.data == data) {
        return i;
        }
    }

    return -1;
}

Node get(Stack* s, size_t index) {
    if (index >= s->size) {
        fatal_err(new_error(STACK_IndexOutofBounds, 
                      __LINE__, __FILE__));

    }

    if (s->size == 0) {
        fatal_err(new_error(STACK_EmptyStack, 
                                  __LINE__, __FILE__));
    }

    size_t i = 0;
    Stack* s_two = new_stack();

    while (i != index) {
        push(s_two, pop(s).data);
        i++;
    }

    Node n = *s->data[s->size - 1];

    while (!is_empty(s_two)) {
        push(s, pop(s_two).data);
    }

    return n;
}

void rev(Stack* s) {
    for (size_t i = 0; i < s->size / 2; ++i) {
        Node* a = s->data[i];
        Node* b = s->data[s->size - i - 1];
        Node temp = *a;

        *a = *b;
        *b = temp;
    }
}

void clear(Stack* s) {
    while (!is_empty(s)) {
        pop(s);
    }
}

void copy(Stack* dest, Stack* src) {
    if (dest == NULL || src == NULL) {
        fatal_err(new_error(STACK_NullStack,
                      __LINE__, __FILE__));
    }
    clear(dest);
    if (dest->size != 0) {
        fatal_err(new_error(STACK_DestStackNotEmpty, 
                      __LINE__, __FILE__));
    }

    for (size_t i = 0; i < src->size; ++i) {
        push(dest, src->data[i]->data);
    }
}

void release(Stack** s_ptr) {
    // Freeing all the memory the Stack used
    Stack* s = *s_ptr;
    clear(s);
    free(s);
    *s_ptr = NULL;
}

#endif /*__STACK_H*/
