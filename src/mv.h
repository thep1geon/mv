#ifndef __MV_H
#define __MV_H

#ifndef STACK_CAP
#define STACK_CAP 10240
#endif //STACK_CAP

#ifndef HEAP_SIZE
#define HEAP_SIZE 10240 // 10 kilobytes of memory should be enough
#endif //HEAP_SIZE

#ifndef NUM_REGISTERS
#define NUM_REGISTERS 10
#endif //NUM_REGISTERS

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "etypes.h"
#include "program.h"
#include "stack.h"
#include "inst.h"
#include "parser.h"
#include "label.h"


typedef struct {
    Program program;
    Stack* stack;
    int heap[HEAP_SIZE];
    long registers[NUM_REGISTERS];
    LabelTable label_table;
    bool halt;
} Mv; // Mirtual Vachine

Mv new_mv(void);
void mv_run(Mv mv, bool debug);
void mv_dump(Mv mv);
void mv_close(Mv mv);
void mv_program_from_file(Mv* mv, const char* file_path);
void mv_set_program(Mv* mv, Program p);
Err mv_execute_inst(Mv* mv, Inst* i, int* ip, bool debug);

Mv new_mv(void) {
    Stack* s = new_stack();
    Mv mv = {.stack = s, .halt = false};
    return mv;
}

void mv_run(Mv mv, bool debug) {
    if (mv.program.size == 0) {
        fatal_err(new_error(MV_ProgramNotFound, 0, 0));
    }

    for (int i = 0; (size_t)i < mv.program.size && !mv.halt; ++i) {
        Err e = mv_execute_inst(&mv, &mv.program.inst[i], &i, debug);

        if (e.type != None) {
            mv.halt = true;
            fatal_err(e);
        }
    }
}

void mv_close(Mv mv) {
    release(&mv.stack);
}

inline void mv_dump(Mv mv) { print(mv.stack); }

void mv_set_program(Mv* mv, Program p) {
    clear(mv->stack);
    mv->program = p;
}

static int count_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1; // Return -1 to indicate an error
    }

    int lines = 0;
    char ch;
    int in_empty_line = 1;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            if (!in_empty_line) {
                lines++;
            }
            in_empty_line = true;
        } else if (ch != ' ' && ch != '\t' && ch != '\r') {
            in_empty_line = false;
        }
    }

    if (!in_empty_line) {
        lines++;
    }

    fclose(file);
    return lines;
}

void mv_program_from_file(Mv* mv, const char* file_path) {
    FILE* f = fopen(file_path, "r");
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    size_t ip = 0;
    int linenumber = 1;
    int program_size = count_lines(file_path);
    Inst* p = (Inst*) malloc(sizeof(Inst) * program_size);

    if (f == NULL) {
        fatal_err(new_error(MV_FileOpenFail, __LINE__, __FILE__)); 
    }
    
    while ((read = getline(&line, &len, f)) > 0) {
        Inst i = parse_line(line);  
        i.line_number = linenumber++;

        if (i.type == LABEL) {
            size_t index = hash(i.literal, PROGRAM_MAX_SIZE);
            Label l = (Label) {.jump_point = ip, .name = i.literal};
            mv->label_table.labels[index] = l;
            mv->label_table.size++;
        }

        if (i.type != EMPTY) {
            p[ip++] = i;
        }

    }

    Program program = new_program(p, program_size, file_path);
    mv_set_program(mv, program);

    free(p);
    fclose(f);
}

Err mv_execute_inst(Mv* mv, Inst* i, int* ip, bool debug) {
    Err e = {.type = None, .line_number = i->line_number, .file = mv->program.file};
 
    if (debug) {
        print_inst(*i);
    }

    switch (i->type) {
    case ADD: {
            if (mv->stack->size < 2) {
                e.type = STACK_StackUnderflow;
                break;
            }
            push(mv->stack, pop(mv->stack).data+pop(mv->stack).data);
        }
        break;
    case SUB: {
            if (mv->stack->size < 2) {
                e.type = STACK_StackUnderflow;
                break;
            }
            push(mv->stack, pop(mv->stack).data-pop(mv->stack).data);
        }
        break;
    case MULT: {
            if (mv->stack->size < 2) {
                e.type = STACK_StackUnderflow;
                break;
            }
            push(mv->stack, pop(mv->stack).data*pop(mv->stack).data);
        }
        break;
    case DIV: {
            if (mv->stack->size < 2) {
                e.type = STACK_StackUnderflow;
                break;
            }

            Node a = pop(mv->stack);
            Node b = pop(mv->stack);

            if (a.data == 0 || b.data == 0) {
                e.type = MV_DivisionByZero;
                break;
            }
            
            push(mv->stack, pop(mv->stack).data/pop(mv->stack).data);
        }
        break;
    case MOD: {
            if (mv->stack->size < 2) {
                e.type = STACK_StackUnderflow;
                break;
            }

            push(mv->stack, pop(mv->stack).data%pop(mv->stack).data);
        }
        break;
    case INC:
        if (i->has_operand) {
            if (i->operand >= 0 && i->operand < (long)mv->stack->size) {
                    (*mv->stack->data[mv->stack->size - 1 - i->operand]).data++;
            } else {
                e.type = STACK_IndexOutofBounds;
                break;
            }
        } else {
            (*mv->stack->data[mv->stack->size - 1]).data++;
        }
        break;
    case DEC:
        if (i->has_operand) {
            if (i->operand >= 0 && i->operand < (long)mv->stack->size) {
                (*mv->stack->data[mv->stack->size - 1 - i->operand]).data--;
            } else {
                e.type = STACK_IndexOutofBounds;
                break;
            }
        } else {
            (*mv->stack->data[mv->stack->size - 1]).data--;
        }
        break;
    case DUPE:
        if (mv->stack->size + 1 == STACK_CAP) {
            e.type = STACK_StackOverflow;
        }

        if (i->has_operand) {
            if (i->operand >= (long)mv->stack->size || i->operand < 0) {
                e.type = STACK_IndexOutofBounds;
                break;
            }

            push(mv->stack, mv->stack->data[mv->stack->size - 1 - i->operand]->data);
        } else {
            push(mv->stack, mv->stack->data[mv->stack->size - 1]->data);
        }
        break;
    case PUSH:
        if (mv->stack->size + 1 == STACK_CAP) {
            e.type = STACK_StackOverflow;
        }

        if (i->literal) {
            push(mv->stack, peek(mv->stack)->data); 
        } else if (i->has_operand) {
            push(mv->stack, i->operand);
        } else {
            e.type = INST_MissingParameters;
        }
        break;
    case PUSH_LIT: {
        if (i->has_operand) {
                for (size_t I = 0; I < (size_t)strlen(i->literal); ++I) {
                    if (mv->stack->size + 1 >= STACK_CAP) {
                        e.type = STACK_StackOverflow;
                    }

                    push(mv->stack, i->literal[I]); 
                }
        } else {
            e.type = INST_MissingParameters;
        }
        }
        break;
    case JMP:
            jump(i, mv->label_table, (size_t*)ip);
        break;
    case JMP_GT:
        if (peek(mv->stack)->data > i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case JMP_GTEQ:
        if (peek(mv->stack)->data >= i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case JMP_LT:
        if (peek(mv->stack)->data < i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case JMP_LTEQ:
        if (peek(mv->stack)->data <= i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case JMP_EQ:
        if (peek(mv->stack)->data == i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case JMP_NEQ:
        if (peek(mv->stack)->data != i->operator) {
            jump(i, mv->label_table, (size_t*)ip);
        } 
        break;
    case STOP:
        e.type = MV_Stop;
        return e;
    case DUMP: {
            if (!i->has_operand) {
                print(mv->stack);
            } else if (i->has_operand) {
                if (i->operand >= 0 && i->operand < (long)mv->stack->size) {
                    print_node(*mv->stack->data[mv->stack->size - 1 - i->operand]);
                } else {
                    e.type = STACK_IndexOutofBounds;
                    break;
                }
            }
        }
        break;
    case PRINT: {
            if (!i->has_operand) {
                print_ascii(mv->stack);
            } else if (i->has_operand) {
                if (i->operand >= 0 && i->operand < (long)mv->stack->size) {
                    print_node_ascii(*mv->stack->data[mv->stack->size - 1 - i->operand], true);
                } else {
                    e.type = STACK_IndexOutofBounds;
                    break;
                }
            }
        }
        break;
    case MOV:
        move(mv->stack, mv->registers, NUM_REGISTERS, *i);
        break;
    case POP: {
            if (mv->stack->size == 0) {
                e.type = STACK_StackUnderflow;
                break;
            }

            if (i->has_operand) {
                Node node = pop(mv->stack);
                if (i->operand >= (long)NUM_REGISTERS || i->operand < 0) {
                    e.type = STACK_IndexOutofBounds;
                    break;
                }

                mv->registers[i->operand] = (long)node.data;
            } else {
                pop(mv->stack);
            }
        }
        break;
    case MEM_READ:
        if (mv->stack->size + 1 >= STACK_CAP) {
            e.type = STACK_StackOverflow;
        } else if (!i->has_operand && !i->literal) {
            e.type = INST_MissingParameters;
        } else if ((i->has_operand && i->operand < 0) || (i->literal != NULL && peek(mv->stack)->data < 0)) {
            e.type = STACK_IndexOutofBounds;
        }

        if (e.type != None) {
            break;        
        }

        if (i->has_operand) {
            push(mv->stack, mv->heap[i->operand]);
        } else if (!i->has_operand && strcmp(i->literal, ".") == 0) {
            push(mv->stack, mv->heap[peek(mv->stack)->data]);
        }
        break;
    case MEM_WRITE:
        if (!i->has_operand && !i->literal && !i->has_operator) {
            e.type = INST_MissingParameters;
        } else if ((i->has_operand && i->operand < 0) || (i->literal != NULL && peek(mv->stack)->data < 0)) {
            e.type = STACK_IndexOutofBounds;
        }

        if (e.type != None) {
            break;        
        }

        if (i->has_operand && i->has_operator && !i->literal) {
            mv->heap[i->operand] = i->operator;
        } else if (!i->has_operand && i->has_operator && i->literal) {
            mv->heap[peek(mv->stack)->data] = i->operator;
        } else if (i->has_operand && !i->has_operator && i->literal) {
            mv->heap[i->operand] = peek(mv->stack)->data;
        } else if (!i->has_operand && !i->has_operator && i->literal) {
            mv->heap[peek(mv->stack)->data] = peek(mv->stack)->data;
        }
        break;
    case LABEL:
        break;
    case EMPTY:
        break;
    case ILL:
        e.type = INST_IllegalInstruction;
    }

    return e;
}

#endif //__MV_H
