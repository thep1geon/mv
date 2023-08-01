#ifndef __MV_H
#define __MV_H


// mv => Mirtual Vachine

#include <stdlib.h>
#include <string.h>
#ifndef STACK_CAP
#define STACK_CAP 1024
#endif

#ifndef NUM_REGISTERS
#define NUM_REGISTERS 10
#endif

#include <stddef.h>
#include <stdio.h>

#include "err.h"
#include "etypes.h"
#include "program.h"
#include "stack.h"
#include "inst.h"
#include "parser.h"


typedef struct {
    Program program;
    Stack* stack;
    long registers[NUM_REGISTERS];
    bool halt;
} Mv; // Mirtual Vachine

// Registers
// mov 0 90 (moving the value 90 into register 0)
// mov 5 (moving the value of register 5 onto the stack)

Mv new_mv();
void mv_run(Mv mv, bool debug);
void mv_dump(Mv mv);
void mv_close(Mv mv);
void mv_program_from_file(Mv* mv, const char* file_path);
void mv_set_program(Mv* mv, Program p);

Mv new_mv() {
    Stack* s = new_stack();
    Mv mv = {.stack = s, .halt = false};
    return mv;
}

void mv_run(Mv mv, bool debug) {
    if (mv.program.size == 0) {
        err(new_error(MV_ProgramNotFound, 
                      "No program for this Mirtual Vachine(tm) found", 
                      __LINE__, __FILE__));
    }

    for (size_t i = 0; i < mv.program.size && !mv.halt; ++i) {
        Err e = execute(mv.stack, mv.program.inst[i], &i, mv.registers, NUM_REGISTERS, debug);

        if (e.type != None) {
            mv.halt = true;
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
    int in_empty_line = 1; // Start with the flag set to true, assuming the first line is empty

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            if (!in_empty_line) {
                lines++;
            }
            in_empty_line = true; // Reset the flag for the next line
        } else if (ch != ' ' && ch != '\t' && ch != '\r') {
            in_empty_line = false; // If we encounter a non-whitespace character, mark the line as not empty
        }
    }

    // If the file doesn't end with a newline, but has content, increment line count
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
    size_t lp = 0;
    int program_size = count_lines(file_path);
    Inst* p = (Inst*) malloc(sizeof(Inst) * program_size);



    if (f == NULL) {
        err(new_error(MV_FileOpenFail, "Failed to open file",
                      __LINE__, __FILE__)); 
    }
    
    while ((read = getline(&line, &len, f)) > 0) {
        Inst i = parse_line(line);  

        if (i.type != EMPTY) {
            p[ip++] = i;
        }
    }

    Program program = new_program(p, program_size);
    mv_set_program(mv, program);

    free(p);
    fclose(f); // Close the file when done reading
}

#endif //__MV_H
