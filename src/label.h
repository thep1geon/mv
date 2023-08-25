#ifndef __LABEL_H
#define __LABEL_H

#include <stdio.h>

#define PROGRAM_MAX_SIZE 1024
typedef unsigned long size_t;

typedef struct {
    int jump_point;
    char* name;
} Label;

typedef struct {
    Label labels[PROGRAM_MAX_SIZE]; // We can't have more labels than the max program size
    size_t size;
} LabelTable;

void print_label(Label l) {
    printf("Label name: %s, jump_point: %d\n", l.name, l.jump_point);
}

// Hash function I found
size_t hash(const char* label_name, size_t array_size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *label_name++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % array_size;
}

#endif //__LABEL_H
