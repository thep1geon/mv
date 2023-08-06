#ifndef __INST_H
#define __INST_H

#include "err.h"
#include "etypes.h"
#include "stack.h"
#include "label.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Instructions go here
typedef enum {
    ADD = 0,
    SUB,
    DIV,
    MOD,
    MULT,
    INC,
    DEC,

    DUPE,
    PUSH,
    PUSH_LIT,
    MOV,
    POP,

    JMP,     // Jump //12
    JMP_GT, //>
    JMP_GTEQ, //>=
    JMP_LT, //<
    JMP_LTEQ, //<=
    JMP_EQ, //==
    JMP_NEQ, // != //18
    
    
    STOP,
    DUMP,
    PRINT,
    LABEL,
    EMPTY, // Empty instruction, used for empty lines in the .mv file
} Inst_Type;

char* operation_to_str(Inst_Type i) {
    switch (i) {
        case ADD: return "ADD";
        case SUB: return "SUB";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case MULT: return "MULT";
        case INC: return "INC";
        case DEC: return "DEC";
        case DUPE: return "DUPE";
        case PUSH: return "PUSH";
        case PUSH_LIT: return "PUSH_LIT";
        case PRINT: return "PRINT";
        case JMP: return "JMP";
        case JMP_GT: return "JMP_GT";
        case JMP_GTEQ: return "JMP_GTEQ";
        case JMP_LT: return "JMP_LT";
        case JMP_LTEQ: return "JMP_LTEQ";
        case JMP_EQ: return "JMP_EQ";
        case JMP_NEQ: return "JMP_NEQ";
        case STOP: return "STOP";
        case DUMP: return "DUMP";
        case EMPTY: return "EMPTY";
        case LABEL: return "LABEL";
        case MOV: return "MOV";
        case POP: return "POP";
        default: return "Unknown Instruction";
    }
}

typedef struct {
    Inst_Type type;
    long operand;
    long operator;
    bool has_operand;
    bool has_operator;
    char* literal;
} Inst;

void print_inst(Inst i) {
    printf("Inst name: %s, operand: %zu, operator: %zu, literal: %s\n", operation_to_str(i.type), i.operand, i.operator, i.literal);
}

// What happens when that instruction is found goes here
static void add(Stack* s) {
    if (s->size < 2) {
        err(new_error(STACK_StackUnderflow, 
                      "Cannot add two elements if the stack doesn't have two elements", 
                      __LINE__, __FILE__));
    }

    Node a = pop(s);
    Node b = pop(s);

    push(s, a.data+b.data);
}

static void sub(Stack* s) {
    if (s->size < 2) {
        err(new_error(STACK_StackUnderflow, 
                      "Cannot sub two elements if the stack doesn't have two elements", 
                      __LINE__, __FILE__));
    }
    Node a = pop(s);
    Node b = pop(s);

    push(s, a.data-b.data);
}

static void mult(Stack* s) {
    if (s->size < 2) {
        err(new_error(STACK_StackUnderflow, 
                      "Cannot multiply two elements if the stack doesn't have two elements", 
                      __LINE__, __FILE__));
    }
    Node a = pop(s);
    Node b = pop(s);

    push(s, a.data*b.data);
}

static void divide(Stack* s) {
    if (s->size < 2) {
        err(new_error(STACK_StackUnderflow, 
                      "Cannot divide two elements if the stack doesn't have two elements", 
                      __LINE__, __FILE__));
    }
    Node a = pop(s);
    Node b = pop(s);

    if (a.data == 0 || b.data == 0) {
        err(new_error(MV_DivisionByZero, "Division by zero error", 
                    __LINE__, __FILE__));
    }

    push(s, a.data/b.data);
}

static void mod(Stack* s) {
    if (s->size < 2) {
        err(new_error(STACK_StackUnderflow, 
                      "Cannot divide two elements if the stack doesn't have two elements", 
                      __LINE__, __FILE__));
    }
    Node a = pop(s);
    Node b = pop(s);

    if (a.data == 0 || b.data == 0) {
        err(new_error(MV_DivisionByZero, "Division by zero error", 
                    __LINE__, __FILE__));
    }

    push(s, a.data%b.data);
}

static void dupe(Stack* s, size_t addr) {
    if (addr >= s->size) {
        err(new_error(STACK_IndexOutofBounds, 
                      "Cannot duplicate element out of bounds", 
                      __LINE__, __FILE__));
    }
    push(s, s->data[s->size - 1 - addr]->data);
}

static void move(Stack* s, long* registers, size_t register_size, Inst i) {
    if (!i.has_operator) {
        if (i.has_operand && i.operand >= 0 && i.operand < (long) register_size) {
            push(s, registers[i.operand]);
            return;
        } else {
            err(new_error(STACK_IndexOutofBounds, "Register not found", 
                          __LINE__, __FILE__));
        }
    } 

    if (i.has_operand && i.has_operator) {
        registers[i.operand] = i.operator;
        return;
    }

    err(new_error(INST_MissingParameters, "Move instruction missing parameters", 
                  __LINE__, __FILE__));
}

void jump(Inst* i, LabelTable lt, size_t* ip) {
    size_t jp;
    if (i->literal && !i->has_operand) {
        Label label = lt.labels[hash(i->literal, PROGRAM_MAX_SIZE)];
        jp = label.jump_point;
    } else if (i->has_operand && i->literal == NULL) {
        jp = i->operand - 1;
    }

    *ip = jp;
}

// The "driver" code for executing the Instructions
Err execute(Stack* s, Inst* i, LabelTable lt, size_t* ip, long* registers, size_t register_size, bool debug) {
    Err e = {.type = None};
 
    if (debug) {
        print_inst(*i);
    }

    switch (i->type) {
    case ADD:
        add(s);
        break;
    case SUB:
        sub(s);
        break;
    case MULT:
        mult(s);
        break;
    case DIV:
        divide(s);
        break;
    case MOD:
        mod(s);
        break;
    case INC:
        if (i->has_operand) {
            if (i->operand >= 0 && i->operand < (long)s->size) {
                    (*s->data[s->size - 1 - i->operand]).data++;
                } else {
                    err(new_error(STACK_IndexOutofBounds, 
                                  "Cannot decrement element outside the stack",
                                  __LINE__, __FILE__));
            }
        } else {
            (*s->data[s->size - 1]).data++;
        }
        break;
    case DEC:
        if (i->has_operand) {
            if (i->operand >= 0 && i->operand < (long)s->size) {
                    (*s->data[s->size - 1 - i->operand]).data--;
            } else {
                err(new_error(STACK_IndexOutofBounds, 
                              "Cannot decrement element outside the stack",
                              __LINE__, __FILE__));
            }
        } else {
            (*s->data[s->size - 1]).data--;
        }
        break;
    case DUPE:
        if (i->has_operand) {
            dupe(s, i->operand);
        } else {
            dupe(s, 0);
        }
        break;
    case PUSH:
        if (i->has_operand) {
            push(s, i->operand);
        } else {
            err(new_error(INST_MissingParameters, "Parameters missing from instuction", 
                          __LINE__, __FILE__));
        }
        break;
    case PUSH_LIT: {
        if (i->has_operand) {
                for (size_t I = 0; I < (size_t)strlen(i->literal); ++I) {
                    push(s, i->literal[I]); 
                }
        } else {
            err(new_error(INST_MissingParameters, "Parameters missing from instuction", 
                          __LINE__, __FILE__));
        }
        }
        break;
    case JMP:
        jump(i, lt, ip);
        break;
    case JMP_GT:
        if (peek(s)->data > i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case JMP_GTEQ:
        if (peek(s)->data >= i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case JMP_LT:
        if (peek(s)->data < i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case JMP_LTEQ:
        if (peek(s)->data <= i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case JMP_EQ:
        if (peek(s)->data == i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case JMP_NEQ:
        if (peek(s)->data != i->operator) {
            jump(i, lt, ip);
        } 
        break;
    case STOP:
        e.type = MV_Stop;
        return e;
    case DUMP: {
            if (!i->has_operand) {
                printf("--------------\n");
                print(s);
                printf("--------------\n");
            } else if (i->has_operand) {
                if (i->operand >= 0 && i->operand < (long)s->size) {
                    print_node(*s->data[s->size - 1 - i->operand]);
                } else {
                    err(new_error(STACK_IndexOutofBounds, "Index out of bounds", 
                                  __LINE__, __FILE__));
                }
            }
        }
        break;
    case PRINT: {
            if (!i->has_operand) {
                printf("--------------\n");
                print_ascii(s);
                printf("--------------\n");
            } else if (i->has_operand) {
                if (i->operand >= 0 && i->operand < (long)s->size) {
                    print_node_ascii(*s->data[s->size - 1 - i->operand], true);
                } else {
                    err(new_error(STACK_IndexOutofBounds, "Index out of bounds", 
                                  __LINE__, __FILE__));
                }
            }
        }
        break;
    case MOV:
        move(s, registers, register_size, *i);
        break;
    case POP: {
            if (i->has_operand) {
                Node node = pop(s);
                if (i->operand >= (long)register_size || i->operand < 0) {
                    err(new_error(STACK_IndexOutofBounds, 
                                  "Cannot pop into register that doesn't exist", 
                                  __LINE__, __FILE__));
                }

                registers[i->operand] = (long)node.data;
            } else {
                pop(s);
            }
        }
        break;
    case LABEL:
        break;
    case EMPTY:
        break;
    }

    if (debug) {
        print(s);
        printf("--------------\n");
    }

    return e;
}
#endif /*__INST_H*/
