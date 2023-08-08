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
    ILL,
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
        case ILL: return "ILL";
        default: return "Unknown Instruction";
    }
}

typedef struct {
    Inst_Type type;
    int line_number;
    long operand;
    long operator;
    bool has_operand;
    bool has_operator;
    char* literal;
} Inst;

void print_inst(Inst i) {
    printf("Inst name: %s, operand: %zu, operator: %zu, literal: %s, Line Number: %d\n", 
           operation_to_str(i.type), 
           i.operand, 
           i.operator, 
           i.literal,
           i.line_number);
}

// What happens when that instruction is found goes here
static void move(Stack* s, long* registers, size_t register_size, Inst i, Err* e, const char* file_path) {
    if (!i.has_operator) {
        if (i.has_operand && i.operand >= 0 && i.operand < (long) register_size) {

            if (s->size + 1 == STACK_CAP) {
                err(new_error(STACK_StackOverflow, "Stack Overflow", 
                              i.line_number, file_path));
            }

            push(s, registers[i.operand]);
            return;
        } else {
            *e = new_error(STACK_IndexOutofBounds, "Register not found", 
                          i.line_number, file_path);
        }
    } 

    else if (i.has_operand && i.has_operator) {
        registers[i.operand] = i.operator;
        return;
    }

    *e = new_error(INST_MissingParameters, "Move instruction missing parameters", 
                  i.line_number, file_path);
}

void jump(Inst* i, LabelTable lt, size_t* ip, Err* e, const char* file_path) {
    size_t jp;
    if (i->literal && !i->has_operand) {
        Label label = lt.labels[hash(i->literal, PROGRAM_MAX_SIZE)];
        
        if (label.name == NULL) {
            printf("Label: %s\n", i->literal);
            *e = new_error(INST_LabelNotFound, "Label not found", 
                          i->line_number, file_path);
        }

        jp = label.jump_point;
    } else if (i->has_operand && i->literal == NULL) {
        jp = i->operand - 1;
    }

    *ip = jp;
}

// The "driver" code for executing the Instructions
Err execute(Stack* s, Inst* i, LabelTable lt, size_t* ip, long* registers, size_t register_size, const char* file_path, bool debug) {
    Err e = {.type = None};
 
    if (debug) {
        print_inst(*i);
    }

    switch (i->type) {
    case ADD: {
            if (s->size < 2) {
                e.line_number = i->line_number;
                e.msg = "Cannot add two elements without two elements";
                e.type = STACK_StackUnderflow;
                e.file = file_path;
                break;
            }
            push(s, pop(s).data+pop(s).data);
        }
        break;
    case SUB: {
            if (s->size < 2) {
                e.line_number = i->line_number;
                e.msg = "Cannot sub two elements without two elements";
                e.type = STACK_StackUnderflow;
                e.file = file_path;
                break;
            }
            push(s, pop(s).data-pop(s).data);
        }
        break;
    case MULT: {
            if (s->size < 2) {
                e.line_number = i->line_number;
                e.msg = "Cannot multiply two elements without two elements";
                e.type = STACK_StackUnderflow;
                e.file = file_path;
                break;
            }
            push(s, pop(s).data*pop(s).data);
        }
        break;
    case DIV: {
            if (s->size < 2) {
                e.line_number = i->line_number;
                e.msg = "Cannot multiply two elements without two elements";
                e.type = STACK_StackUnderflow;
                e.file = file_path;
                break;
            }

            Node a = pop(s);
            Node b = pop(s);

            if (a.data == 0 || b.data == 0) {
                e = new_error(MV_DivisionByZero, "Division by zero error", 
                            i->line_number, file_path);
                break;
            }
            
            push(s, pop(s).data/pop(s).data);
        }
        break;
    case MOD: {
            if (s->size < 2) {
                e.line_number = i->line_number;
                e.msg = "Cannot multiply two elements without two elements";
                e.type = STACK_StackUnderflow;
                e.file = file_path;
                break;
            }

            push(s, pop(s).data%pop(s).data);
        }
        break;
    case INC:
        if (i->has_operand) {
            if (i->operand >= 0 && i->operand < (long)s->size) {
                    (*s->data[s->size - 1 - i->operand]).data++;
                } else {
                    e = new_error(STACK_IndexOutofBounds, 
                                  "Cannot decrement element outside the stack",
                                  i->line_number, file_path);
                    break;
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
                e = new_error(STACK_IndexOutofBounds, 
                              "Cannot decrement element outside the stack",
                              i->line_number, file_path);
                break;
            }
        } else {
            (*s->data[s->size - 1]).data--;
        }
        break;
    case DUPE:
        if (s->size + 1 == STACK_CAP) {
            err(new_error(STACK_StackOverflow, "Stack Overflow", 
                          i->line_number, file_path));
        }

        if (i->has_operand) {
            if (i->operand >= (long)s->size) {
                e = new_error(STACK_IndexOutofBounds, 
                              "Cannot duplicate element out of bounds", 
                              i->line_number, file_path);
                break;
            }

            push(s, s->data[s->size - 1 - i->operand]->data);
        } else {
            push(s, s->data[s->size - 1]->data);
        }
        break;
    case PUSH:
        if (i->has_operand) {
            if (s->size + 1 == STACK_CAP) {
                err(new_error(STACK_StackOverflow, "Stack Overflow", 
                              i->line_number, file_path));
            }

            push(s, i->operand);
        } else {
            e = new_error(INST_MissingParameters, "Parameters missing from instuction", 
                          i->line_number, file_path);
            break;
        }
        break;
    case PUSH_LIT: {
        if (i->has_operand) {
                for (size_t I = 0; I < (size_t)strlen(i->literal); ++I) {
                    if (s->size + 1 >= STACK_CAP) {
                        err(new_error(STACK_StackOverflow, "Stack Overflow", 
                                      i->line_number, file_path));
                    }

                    push(s, i->literal[I]); 
                }
        } else {
            e = new_error(INST_MissingParameters, "Parameters missing from instuction", 
                          __LINE__, file_path);
            break;
        }
        }
        break;
    case JMP:
        jump(i, lt, ip, &e, file_path);
        break;
    case JMP_GT:
        if (peek(s)->data > i->operator) {
            jump(i, lt, ip, &e, file_path);
        } 
        break;
    case JMP_GTEQ:
        if (peek(s)->data >= i->operator) {
            jump(i, lt, ip, &e, file_path);
        } 
        break;
    case JMP_LT:
        if (peek(s)->data < i->operator) {
            jump(i, lt, ip, &e, file_path);
        } 
        break;
    case JMP_LTEQ:
        if (peek(s)->data <= i->operator) {
            jump(i, lt, ip, &e, file_path);
        } 
        break;
    case JMP_EQ:
        if (peek(s)->data == i->operator) {
            jump(i, lt, ip, &e, file_path);
        } 
        break;
    case JMP_NEQ:
        if (peek(s)->data != i->operator) {
            jump(i, lt, ip, &e, file_path);
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
                    e = new_error(STACK_IndexOutofBounds, "Index out of bounds", 
                                  i->line_number,  file_path);
                    break;
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
                    e = new_error(STACK_IndexOutofBounds, "Index out of bounds", 
                                  i->line_number, file_path);
                    break;
                }
            }
        }
        break;
    case MOV:
        move(s, registers, register_size, *i, &e, file_path);
        break;
    case POP: {
            if (i->has_operand) {
                Node node = pop(s);
                if (i->operand >= (long)register_size || i->operand < 0) {
                    e = new_error(STACK_IndexOutofBounds, 
                                  "Cannot pop into register that doesn't exist", 
                                  i->line_number, file_path);
                    break;
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
    case ILL:
        err(new_error(INST_IllegalInstruction, "Instruction not found", i->line_number, file_path));
    }

    return e;
}
#endif /*__INST_H*/
