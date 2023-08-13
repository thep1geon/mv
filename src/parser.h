#ifndef __PARSER_H
#define __PARSER_H

#include "err.h"
#include "etypes.h"
#include "inst.h"
#include "stack.h"
#include "label.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

InstType str_to_type(const char* str) {
    if (strcmp(str, "func") == 0) {return FUNC;}

    else if (strlen(str) >= 1) {
        if (str[strlen(str)-1] == ':') {return LABEL;}
    }

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

    else if (strcmp(str, "mem_read") == 0) {return MEM_READ;}
    else if (strcmp(str, "mem_write") == 0) {return MEM_WRITE;}

    else if (strcmp(str, "ret") == 0) {return RET;}
    else if (strcmp(str, "call") == 0) {return CALL;}

    else if (strcmp(str, "include") == 0) {return INCLUDE;}
    
    else if (strcmp(str, "") == 0) { return EMPTY;}

    printf("Unknown Instruction: %s\n", str);

    return ILL;
}

Inst parse_line(char* line) {
    // remove_newline(line);
    Inst i = {0};
    i.type = EMPTY;
    i.has_operand = false;
    i.has_operator = false;
    i.literal = malloc(1000);
    i.line_number = 0;

    if (line[0] == ';') {
        return i;
    }

    char* new_line = (char*)malloc(strlen(line)+1);
    if (new_line != NULL) {
        size_t line_length = strlen(line);
        for (size_t I = 0; I < line_length; ++I) {
            new_line[I] = line[I];
        }
        new_line[line_length] = '\0';
    } else {
        fprintf(stderr, "Malloc Fail\n");
    }

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
                i.literal = substr(line, 10, strlen(line)-2);
            }

            if (i.type == INCLUDE) {
                i.literal = substr(operand, 1, strlen(operand)-1);
            }

            if (i.type == FUNC) {
                i.literal = substr(operand, 0, strlen(operand)-1);
            }

            if ((i.type >= JMP && i.type <= JMP_NEQ) || (i.type == CALL)) {
                i.literal = substr(operand, 0, strlen(operand));
                i.has_operand = false;
            } else {
                if (!isnum(operand) && operand[0] != '-') {
                    i.operand = operand[0]; 
                } else {
                    i.operand = atoi(operand);
                }
                i.has_operand = true;
            }

            if (operand[0] == '.') {
                i.literal = ".";
                i.has_operand = false;
            }

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

            if (operator[0] == '.') {
                i.literal = ".";
                i.has_operator = false;
            }
        } else {
            break;
        }

        token = strtok(NULL, " ");
    }

    if (i.type == LABEL) {
        i.literal = substr(line, 0, strlen(line)-2);
    }

    free(new_line);
    return i;
}

#endif //__PARSER_H
