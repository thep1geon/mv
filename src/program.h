#ifndef __PROGRAM_H
#define __PROGRAM_H

#define PROGRAM_LEN(p) (sizeof((p))/sizeof((p[0])))

#include "inst.h"
#include <stddef.h>
#include <string.h>

#ifndef PROGRAM_MAX_SIZE
#define PROGRAM_MAX_SIZE 1024
#endif

typedef struct {
    Inst inst[PROGRAM_MAX_SIZE];
    size_t size;
} Program;

// Like a constructor
Program new_program(Inst* i, size_t program_size) {
    Program p = {};    
    memcpy(p.inst, i, sizeof(i[0]) * program_size);
    p.size = program_size;
    return p;
}

#endif //__PROGRAM_H
