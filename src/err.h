#ifndef __ERR_H
#define __ERR_H

#include "etypes.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    ErrType type;
    size_t line_number;
    const char* file;
    const char* msg;
} Err;

Err new_error(ErrType type, const char* msg, size_t line_number, const char* file_name) {
    return (Err){.type = type, .line_number = line_number, .msg = msg, .file = file_name};
}

void err(Err e) {
    fprintf(stderr, "%s ERROR at line %zu: %s\n", 
            e.file, e.line_number, err_type_to_str(e.type));

    fprintf(stderr, "Message: %s\n", e.msg);
    exit(e.type);
}

void non_failing_err(Err e) {
    fprintf(stderr, "NON FATAL ERROR at line %zu: %s\n", 
            e.line_number, err_type_to_str(e.type));

    fprintf(stderr, "Message: %s\n", e.msg);
}

#endif //__ERR_H
