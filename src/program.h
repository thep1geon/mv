#ifndef __PROGRAM_H
#define __PROGRAM_H

#define PROGRAM_LEN(p) (sizeof((p))/sizeof((p[0])))

#include "inst.h"
#include "parser.h"
#include <stddef.h>
#include <string.h>

#ifndef PROGRAM_MAX_SIZE
#define PROGRAM_MAX_SIZE 1024
#endif

typedef struct {
    Inst inst[PROGRAM_MAX_SIZE];
    size_t size;
    const char* file;
} Program;

// Like a constructor
Program new_program(Inst* i, size_t program_size, const char* file_name) {
    Program p = {};    
    memcpy(p.inst, i, sizeof(i[0]) * program_size);
    p.file = substr(file_name, 0, strlen(file_name));
    p.size = program_size;
    return p;
}

#endif //__PROGRAM_H
