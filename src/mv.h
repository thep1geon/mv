#ifndef __MV_H
#define __MV_H

#ifndef INCLUDE_PATH
#define INCLUDE_PATH "/home/magic/mv/include/"
#endif //INCLUDE_PATH

#ifndef HEAP_SIZE
#define HEAP_SIZE 10240 // 10 kilobytes of memory should be enough
#endif //HEAP_SIZE

#ifndef NUM_REGISTERS
#define NUM_REGISTERS 10 // I really don't know how many registers are needed
#endif //NUM_REGISTERS

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"
#include "etypes.h"
#include "program.h"
#include "stack.h"
#include "inst.h"
#include "parser.h"
#include "label.h"

// The Virtual Machine
typedef struct {
    Program program;
    Stack* stack;
    Stack* call_stack;
    int heap[HEAP_SIZE];
    int heap_size;
    long registers[NUM_REGISTERS];
    LabelTable label_table;
    bool halt;
} Mv; // Mirtual Vachine

// Functions related to the Virtual Machine
Mv new_mv(void);
void mv_run(Mv mv, bool debug);
void mv_dump(Mv mv);
void mv_close(Mv mv);
void mv_program_from_file(Mv* mv, const char* file_path);
void mv_set_program(Mv* mv, Program p);
Err mv_execute_inst(Mv* mv, Inst* i, size_t* ip, bool debug);
ErrType mv_include_file(Mv* mv, char* file_path);
size_t mv_find_memory(Mv mv, size_t length);

Mv new_mv(void) {
    Stack* s = new_stack();
    Stack* call_stack = new_stack();
    Mv mv = {.stack = s, .call_stack = call_stack, .halt = false, .heap_size = HEAP_SIZE};
    return mv;
}

// Running the program in the vm's memory
void mv_run(Mv mv, bool debug) {
    if (mv.program.size == 0) {
        fatal_err(new_error(MV_ProgramNotFound, 0, 0));
    }

    for (size_t i = 0; i < mv.program.size && !mv.halt; ++i) {
        Err e = mv_execute_inst(&mv, &mv.program.inst[i], &i, debug);
        // Passing the reference to the loop counter so we can change it

        if (e.type != None) {
            mv.halt = true;
            // Handle any errors produced by executing the last instruction
            fatal_err(e);
        }
    }
}

void mv_close(Mv mv) {
    release(&mv.stack);
    release(&mv.call_stack);

    free(mv.program.file);

    for (int i = 0; i < mv.heap_size; ++i) {
        mv.heap[i] = 0;
        // NULL out the heap
    }

    for (size_t i = 0; i < mv.program.size; ++i) {
        Inst* inst = &mv.program.inst[i];
        if (inst->literal && inst->was_literal_alloced) {
            free(inst->literal);
            inst->literal = NULL;
        }
    }
}

inline void mv_dump(Mv mv) { print(mv.stack); }

void mv_set_program(Mv* mv, Program p) {
    clear(mv->stack);
    mv->program = p;
}

static int count_empty_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1;
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
    int program_size = count_empty_lines(file_path);
    Inst* p = (Inst*) malloc(sizeof(Inst) * program_size);

    if (f == NULL) {
        fatal_err(new_error(MV_FileOpenFail, __LINE__, __FILE__)); 
    }
    
    while ((read = getline(&line, &len, f)) > 0) {
        Inst i = parse_line(line);  
        i.line_number = linenumber++;

        if (i.type == LABEL || i.type == FUNC) {
            // Functions are just fancy labels
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

bool file_exists(const char* path) {
    return access(path, F_OK) != -1;
}

ErrType mv_include_file(Mv* mv, char* file_path) {
    char* include_path = INCLUDE_PATH;
    // The first path we look in
    char file_include_path[100];
    snprintf(file_include_path, sizeof(file_include_path), "%s%s", include_path, file_path);

    int old_program_size = mv->program.size;
    int new_program_size = old_program_size;

    if (file_exists(file_include_path)) {
        // Check the include path first
        new_program_size += count_empty_lines(file_include_path); 
    } else if (file_exists(file_path)) {
        // then the current directory
        new_program_size += count_empty_lines(file_path);
    } else {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return -1; 
    }

    mv->program.size = new_program_size;
    // Update the program size

    FILE* f;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t ip = old_program_size;
    
    int linenumber = mv->program.inst[old_program_size-1].line_number; // Finding the line number of the last instruction
    
    if (file_exists(file_include_path)) {
        f = fopen(file_include_path, "r");

        if (f == NULL) {
            return MV_FileOpenFail;
        }
    } else {
        f = fopen(file_path, "r");

        if (f == NULL) {
            return MV_FileOpenFail;
        }
    }

    while ((read = getline(&line, &len, f)) > 0) {
        Inst i = parse_line(line);  
        i.line_number = linenumber++;

        if (i.type == LABEL || i.type == FUNC) {
            size_t index = hash(i.literal, PROGRAM_MAX_SIZE);
            Label l = (Label) {.jump_point = ip, .name = i.literal};

            mv->label_table.labels[index] = l;
            mv->label_table.size++;
        }

        if (i.type != EMPTY) {
            mv->program.inst[ip++] = i;
        }

    }

    fclose(f);
    return None;
}

// Returns the address of the first availabe space of memory of n length
size_t mv_find_memory(Mv mv, size_t length) {
    for (size_t i = 0; i < mv.heap_size - length; i++) {
        size_t j;
        for (j = 0; j < length; j++) {
            if (mv.heap[i + j] != 0) {
                break; 
            }

            if (mv.heap[i + j] == 0 && mv.heap[i+j-1] != 0) {
                break;
            }
        }

        if (j == length) {
            return i;
        }
    }

    return -1;
    // -1 if no space was found
}

Err mv_execute_inst(Mv* mv, Inst* i, size_t* ip, bool debug) {
    // Set up an error at the beginning
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
            
            push(mv->stack, a.data/b.data);
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
            // Decrement the top of the stack if no index is provided
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
            break;
        } else if (i->has_literal && mv->stack->size < 1) {
            e.type = STACK_EmptyStack;
            break;
        }

        if (i->has_operand) {
            push(mv->stack, i->operand);
        } else if (i->has_literal) {
            push(mv->stack, peek(mv->stack)->data); 
        } else {
            e.type = INST_MissingParameters;
        }
        break;
    case PUSH_LIT: {
        if (i->has_operand) {
            for (size_t j = 0; j < (size_t)strlen(i->literal); ++j) {
                if (mv->stack->size+1 >= STACK_CAP) {
                    e.type = STACK_StackOverflow;
                    break;
                }

                push(mv->stack, i->literal[j]); 
            }
        } else {
            e.type = INST_MissingParameters;
        }
        }
        break;
    case JMP:
            e.type = jump(i, mv->label_table, ip);
        break;
    case JMP_GT:
        if (peek(mv->stack)->data > i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case JMP_GTEQ:
        if (peek(mv->stack)->data >= i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case JMP_LT:
        if (peek(mv->stack)->data < i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case JMP_LTEQ:
        if (peek(mv->stack)->data <= i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case JMP_EQ:
        if (peek(mv->stack)->data == i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case JMP_NEQ:
        if (peek(mv->stack)->data != i->operator) {
            e.type = jump(i, mv->label_table, ip);
        } 
        break;
    case STOP:
        e.type = MV_Stop;
        break;
    case DUMP: {
            // Print the stack as numbers
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
            // Print the stack as characters
            if (!i->has_operand) {
                print_ascii(mv->stack);
            } else if (i->has_operand) {
                if (i->operand >= 0 && i->operand < (long)mv->stack->size) {
                    print_node_ascii(*mv->stack->data[mv->stack->size - 1 - i->operand]);
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
                e.type = STACK_EmptyStackPop;
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
        } else if ((i->has_operand && i->operand < 0) || 
                    (i->literal != NULL && mv->stack->size >= 1 && peek(mv->stack)->data < 0)) {
            e.type = STACK_IndexOutofBounds;
        }

        if (e.type != None) {
            break;        
        }

        if (i->has_operand) {
            push(mv->stack, mv->heap[i->operand]);
        } else if (!i->has_operand && strcmp(i->literal, ".") == 0 && mv->stack->size >= 1) {
            push(mv->stack, mv->heap[peek(mv->stack)->data]);
        }
        break;
    case MEM_WRITE:
        if (!i->has_operand && !i->has_literal && !i->has_operator) {
            e.type = INST_MissingParameters;
        } else if ((i->has_operand && i->operand < 0) || 
                    (i->has_literal && mv->stack->size >= 1 && peek(mv->stack)->data < 0)) {
            e.type = STACK_IndexOutofBounds;
        }

        if (e.type != None) {
            break;        
        }

        if (i->has_operand && i->has_operator) {
            mv->heap[i->operand] = i->operator;
        } else if (!i->has_operand && i->has_operator && i->has_literal) {
            mv->heap[peek(mv->stack)->data] = i->operator;
        } else if (i->has_operand && !i->has_operator && i->has_literal) {
            mv->heap[i->operand] = peek(mv->stack)->data;
        } else if (!i->has_operand && !i->has_operator && i->has_literal) {
            mv->heap[peek(mv->stack)->data] = peek(mv->stack)->data;
        }
        break;
    case CALL:
        // Push the return value onto the call stack
        push(mv->call_stack, *ip);
        jump(i, mv->label_table, (size_t*)ip); // jump to the function
        break;
    case RET: {
        assert(!is_empty(mv->call_stack));
        *ip = pop(mv->call_stack).data;
        }
        break;
    case FUNC:
        if (is_empty(mv->call_stack)) {
            // We reached a function without calling it
            // So we need to skip over it 
            Inst* inst = &mv->program.inst[*ip];
            
            while (inst->type != RET) {
                inst++;
                *ip += 1;
            }
        }
        break;
    case INCLUDE: {
        e.type = mv_include_file(mv, i->literal); 
    }
        break;
    case SIZE:
        if (mv->stack->size + 1 >= STACK_CAP) {
            e.type = STACK_StackOverflow;
            break;
        }
        
        push(mv->stack, mv->stack->size);
        break;
    case SWAP: {
        // Swap the two elements on the Stack
        // sets the registers back to zero after 
        if (mv->stack->size >= 2) {
            mv->registers[8] = pop(mv->stack).data;
            mv->registers[7] = pop(mv->stack).data;
            
            push(mv->stack, mv->registers[8]);
            push(mv->stack, mv->registers[7]);

            mv->registers[8] = 0;
            mv->registers[7] = 0;
        } else {
            e.type = STACK_StackUnderflow;
        }
        }
        break;
    case STR: {
        size_t len = strlen(i->literal);
        // Find some memory for the string
        int begin = mv_find_memory(*mv, len+1); 
        if (begin != -1) {
            size_t j;
            for (j = begin; j < begin + len; ++j) {
                mv->heap[j] = *i->literal++;
            }  
            mv->heap[++j] = '\0';
            push(mv->stack, len); // push the length
            push(mv->stack, begin); // and then the pointer to the beginning of the string
        } else {
            e.type = MV_MemFail;
        }
    }
        break;
    case ARR: {
        if (!i->has_operand) {
            e.type = INST_MissingParameters;
            break;
        }
         
        int size = i->operand;

        int ptr = mv_find_memory(*mv, size+1);
        if (ptr == -1) {
            e.type = MV_MemFail;
            break;
        }

        int j;
        for (j = ptr; j < ptr + size; ++j) {
            if (i->has_operator) {
                mv->heap[j] = i->operator;
            } else {
                mv->heap[j] = -1;
            }
        }
        mv->heap[++j] = '\0';

        push(mv->stack, ptr);
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
// 630 lines of code 🇱 👊
