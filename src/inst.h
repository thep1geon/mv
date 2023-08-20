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
    EMPTY = 0, // Empty instruction, used for empty lines in the .mv file

    ADD,
    SUB,
    DIV,
    MOD,
    MULT,
    INC,
    DEC,

    DUPE,
    PUSH,
    PUSH_LIT,
    INCLUDE,
    MOV,
    POP,

    JMP,     // Jump //12
    JMP_GT, //>
    JMP_GTEQ, //>=
    JMP_LT, //<
    JMP_LTEQ, //<=
    JMP_EQ, //==
    JMP_NEQ, // != //18
    
    MEM_READ,
    MEM_WRITE,
    STR,
    ARR,
    
    RET,
    CALL,

    STOP,
    DUMP,
    PRINT,
    SIZE,
    SWAP,

    LABEL,
    FUNC,

    ILL,
} InstType;

char* operation_to_str(InstType i) {
    switch (i) {
        case ADD:       return "ADD";
        case SUB:       return "SUB";
        case DIV:       return "DIV";
        case MOD:       return "MOD";
        case MULT:      return "MULT";
        case INC:       return "INC";
        case DEC:       return "DEC";
        case DUPE:      return "DUPE";
        case PUSH:      return "PUSH";
        case PUSH_LIT:  return "PUSH_LIT";
        case PRINT:     return "PRINT";
        case JMP:       return "JMP";
        case JMP_GT:    return "JMP_GT";
        case JMP_GTEQ:  return "JMP_GTEQ";
        case JMP_LT:    return "JMP_LT";
        case JMP_LTEQ:  return "JMP_LTEQ";
        case JMP_EQ:    return "JMP_EQ";
        case JMP_NEQ:   return "JMP_NEQ";
        case MEM_READ:  return "MEM_READ";
        case MEM_WRITE: return "MEM_WRITE";
        case RET:       return "RET";
        case STR:       return "STR";
        case CALL:      return "CALL";
        case STOP:      return "STOP";
        case DUMP:      return "DUMP";
        case SIZE:      return "SIZE";
        case SWAP:      return "SWAP";
        case EMPTY:     return "EMPTY";
        case LABEL:     return "LABEL";
        case FUNC:      return "FUNC";
        case MOV:       return "MOV";
        case POP:       return "POP";
        case INCLUDE:   return "INCLUDE";
        case ILL:       return "ILL";
        default:        return "Unknown Instruction";
    }
}

// Each instruction has these 
typedef struct {
    InstType type;
    int line_number;
    long operand;
    long operator;
    bool has_operand;
    bool has_operator;
    char* literal;
    bool has_literal;
    bool was_literal_alloced;
} Inst;

void print_inst(Inst i) {
    printf("Inst type: %s, operand: %zu, has_operand: %d, operator: %zu, has_operator: %d, literal: %s, has_literal: %d, Line Number: %d\n", 
           operation_to_str(i.type), 
           i.operand, 
           i.has_operand,
           i.operator, 
           i.has_operator,
           i.literal,
           i.has_literal,
           i.line_number);
}

// Moving data in and out of the registers
ErrType move(Stack* s, long* registers, size_t register_size, Inst i) {
    // Registers is just an array of longs
    if (i.has_operand && i.has_operator) {
        registers[i.operand] = i.operator;
        return None;
    }

    if (i.has_literal && i.has_operand && s->size >= 1) {
        registers[i.operand] = peek(s)->data;
        return None;
    } else if (i.has_literal && s->size <= 0) {
        return STACK_EmptyStack;
    }

    if (!i.has_operator) {
        if (i.has_operand && i.operand >= 0 && i.operand < (long) register_size) {

            if (s->size + 1 == STACK_CAP) {
                return STACK_StackOverflow;
            }

            push(s, registers[i.operand]);
            return None;
        } else {
            return STACK_IndexOutofBounds;
        }
    } 

    return INST_MissingParameters;
}

// Jumping around in the program
ErrType jump(Inst* i, LabelTable lt, size_t* ip) {
    size_t jp;
    if (i->has_literal && !i->has_operand) {
        Label label = lt.labels[hash(i->literal, PROGRAM_MAX_SIZE)];
        // Get the label from the label table
        
        if (label.name == NULL) {
            // If the label is not found
            printf("Label: %s\n", i->literal);
            return INST_LabelNotFound;
        }

        jp = label.jump_point;
    } else if (i->has_operand && !i->has_literal) {
        jp = i->operand - 1;
    }

    *ip = jp;
    return None; // return a none error
}
#endif /*__INST_H*/
