#ifndef __PARSER_H
#define __PARSER_H

#include "err.h"
#include "etypes.h"
#include "inst.h"
#include "stack.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void remove_newline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

bool isnum(const char* str) {
    if (str == NULL || *str == '\0') {
        return false;  // Empty string or NULL pointer
    }

    while (*str) {
        if (!isdigit(*str)) {
            return false;  // Non-digit character found
        }
        str++;
    }

    return true;  // All characters are digits
}

char* substr(const char* src, int start, int end) {
    int input_len = strlen(src);
    char* dest = malloc(end - start + 1);

    if (start > input_len) {
        return NULL;
    }

    strncpy(dest, src + start, end);
    dest[end - start] = '\0';
    return dest;
}

Inst_Type str_to_type(const char* str) {
    if (strcmp(str, "add") == 0) { return ADD;}
    else if (strcmp(str, "sub") == 0) { return SUB;}
    else if (strcmp(str, "div") == 0) { return DIV;}
    else if (strcmp(str, "mod") == 0) { return MOD;}
    else if (strcmp(str, "mult") == 0) { return MULT;}
    else if (strcmp(str, "inc") == 0) { return INC;}
    else if (strcmp(str, "dec") == 0) { return DEC;}

    else if (strcmp(str, "push") == 0) { return PUSH;}
    else if (strcmp(str, "push_lit") == 0) { return PUSH_LIT;}
    else if (strcmp(str, "dupe") == 0) { return DUPE;}
    else if (strcmp(str, "pop") == 0) {return POP;}

    else if (strcmp(str, "stop") == 0) { return STOP;}
    else if (strcmp(str, "dump") == 0) { return DUMP;}
    else if (strcmp(str, "print") == 0) { return PRINT;}

    else if (strcmp(str, "mov") == 0) {return MOV;}

    else if (strcmp(str, "jmp") == 0) { return JMP;}
    else if (strcmp(str, "jmp_gt") == 0) { return JMP_GT;}
    else if (strcmp(str, "jmp_gteq") == 0) { return JMP_GTEQ;}
    else if (strcmp(str, "jmp_lt") == 0) { return JMP_LT;}
    else if (strcmp(str, "jmp_lteq") == 0) { return JMP_LTEQ;}
    else if (strcmp(str, "jmp_eq") == 0) { return JMP_EQ;}
    else if (strcmp(str, "jmp_neq") == 0) { return JMP_NEQ;}
    else if (strcmp(str, "") == 0) { return EMPTY;}

    printf("Unknown Instruction: %s\n", str);

    err(new_error(INST_IllegalInstruction, "Instruction not found", 
                  __LINE__, __FILE__));
    return STOP;
}

Inst parse_line(char* line) {
    Inst i = {0};
    i.type = EMPTY;
    i.has_operand = false;
    i.has_operator = false;

    if (line[0] == ';') {
        return i;
    }

    char new_line[strlen(line)];
    memcpy(new_line, line, strlen(line)+1);

    char* token = strtok(new_line, " ");
    char* type = NULL;
    char* operand = NULL;
    char* operator = NULL;

    while (token != NULL) {
        if (type == NULL) {
            type = token;
            remove_newline(type);
            i.type = str_to_type(type);
        } else if (operand == NULL) {
            operand = token;
            remove_newline(operand);

            if (operand[0] == ';') {
                return i;
            }
                    
            if (i.type == PUSH_LIT) {
                i.literal = substr(line, 9, strlen(line));
            }

            if (!isnum(operand)) {
                i.operand = operand[0]; 
            } else {
                i.operand = atoi(operand);
            }

            i.has_operand = true;
        } else if (operator == NULL) {
            operator = token;
            remove_newline(operator);

            if (operator[0] == ';') {
                return i;
            }

            if (!isnum(operator)) {
                i.operator = (char)operator[0];
            } else {
                i.operator = atoi(operator);
            }

            i.has_operator = true;
        }

        token = strtok(NULL, " ");

    }

    return i;
}

#endif //__PARSER_H
