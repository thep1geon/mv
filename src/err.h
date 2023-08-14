#ifndef __ERR_H
#define __ERR_H

#include "etypes.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Errors as types
typedef struct {
    ErrType type;
    size_t line_number; // This is for where the err originated from
    const char* file; // File name
} Err;

Err new_error(ErrType type, size_t line_number, const char* file_name) {
    return (Err){.type = type, .line_number = line_number, .file = file_name};
}

void fatal_err(Err e) {
    fprintf(stderr, "File: %s >> ERROR!! (Line %zu) -> %s\n", 
            e.file, e.line_number, err_type_to_str(e.type));

    fprintf(stderr, "Exit Code: %d\n", e.type);
    // Each error type has it's own error code
    exit(e.type);
}

void non_failing_err(Err e) {
    fprintf(stderr, "File: %s >> NON FATAL ERROR! (Line %zu) ->  %s\n", 
            e.file, e.line_number, err_type_to_str(e.type));
}

#endif //__ERR_H
