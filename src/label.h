#ifndef __LABEL_H
#define __LABEL_H

#include <stddef.h>
#include <stdio.h>

#define PROGRAM_MAX_SIZE 1024

typedef struct {
    int jump_point;
    size_t index;
    char* name;
} Label;

typedef struct {
    Label labels[PROGRAM_MAX_SIZE];
    size_t size;
} LabelTable;

void print_label(Label l) {
    printf("Label name: %s, jump_point: %d\n", l.name, l.jump_point);
}

size_t hash(const char* label_name, size_t array_size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *label_name++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % array_size;
}

#endif //__LABEL_H
