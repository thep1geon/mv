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

typedef struct {
    int data;
} Node;

Node* node_alloc(int data);
void print_node(Node n);

Node* node_alloc(int data) {
    Node *node = (Node *)(malloc(sizeof(Node)));

    if (node == NULL) {
        err(new_error(STACK_NodeAllocFail, 
                      "Failed to allocate memory for a node", 
                      __LINE__, __FILE__));
    }

    node->data = data;

    return node;
}

inline void print_node(Node n) { printf("%d\n", n.data); }

//Stack stuff

typedef struct {
    Node* data[STACK_CAP];
    size_t size;
} Stack;

Stack* new_stack();
void print(Stack *stack);
void push(Stack* s, int data);
void push_node(Stack* s, Node* n);
Node pop(Stack* s);
Node* peek(Stack* s);
bool is_empty(Stack* s);
int search(Stack *s, int data);
Node get(Stack *s, size_t index);
void rev(Stack *s);
void clear(Stack *s);
void copy(Stack *dest, Stack *src);

Stack* new_stack() {
    Stack* s = (Stack*)(malloc(sizeof(Stack)));

    if (!s) {
        err(new_error(STACK_StackAllocFail, 
                      "Falied to allocate memory for a stack", 
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

    for (int i = stack->size - 1; i >= 0; --i) {
        print_node(*stack->data[i]);
    }
}

//    <- push
// 81
// 23
// 42

void push(Stack* s, int data) {
    if (s->size == STACK_CAP) {
        err(new_error(STACK_StackOverflow, "Stack Overflow", 
                      __LINE__, __FILE__));
    }

    Node* new_node = node_alloc(data);
    s->data[s->size] = new_node;
    s->size++;
}

void push_node(Stack* s, Node* n) {
    if (s->size == STACK_CAP) {
        err(new_error(STACK_StackOverflow, "Stack Overflow", 
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
        err(new_error(STACK_EmptyStackPop, "Empty stack, cannot pop", 
                      __LINE__, __FILE__));
    }

    Node popped_node = *s->data[--s->size];
    free(s->data[s->size]);
    return popped_node;
}

Node* peek(Stack* s) {
    if (s->size == 0) {
        non_failing_err(new_error(STACK_EmptyStackPeek, 
                                  "Empty stack, cannot peek", 
                                  __LINE__, __FILE__));
        return NULL;
    }

    return s->data[s->size - 1];
}

inline bool is_empty(Stack* s) { return s->size == 0; }

int search(Stack* s, int data) {
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
        err(new_error(STACK_IndexOutofBounds, 
                      "Index out of bounds", __LINE__, __FILE__));

    }

    if (s->size == 0) {
        non_failing_err(new_error(STACK_EmptyStack, 
                                  "Nothin to get from an empty stack", 
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
        err(new_error(STACK_NullStack, "Cannot copy from/to null stack",
                      __LINE__, __FILE__));
    }
    clear(dest);
    if (dest->size != 0) {
        err(new_error(STACK_DestStackNotEmpty, 
                      "Dest stack not emptyied before copy", 
                      __LINE__, __FILE__));
    }

    for (size_t i = 0; i < src->size; ++i) {
        push(dest, src->data[i]->data);
    }
}

void release(Stack** s_ptr) {
    Stack* s = *s_ptr;
    clear(s);
    free(s);
    *s_ptr = NULL;
}

#endif /*__STACK_H*/
