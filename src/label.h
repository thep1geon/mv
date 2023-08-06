#ifndef __LABEL_H
#define __LABEL_H

#include "program.h"

typedef struct {
    int jump_point;
    char* name;
} Label;

typedef struct {
    Label labels[PROGRAM_MAX_SIZE];
    size_t size;
} LabelTable;

#endif //__LABEL_H
