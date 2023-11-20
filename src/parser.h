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

bool str_eq(const char* one, const char* two) {
    if (strlen(one) != strlen(two)) {
        return false;
    }

    for (size_t i = 0; i < strlen(one); ++i) {
        if (one[i] != two[i]) {
            return false;
        }
    }

    return true;
}

// (I don't think) This code is memory safe
char* substr(const char* src, int start, int end) {
    int input_len = strlen(src);
    int subtr_len = end - start + 1;

    if (start > input_len) {
        return NULL;
    }

    char* dest = malloc(subtr_len);

    if (dest == NULL) {
        return NULL;
    }

    strncpy(dest, src + start, subtr_len - 1);
    dest[subtr_len - 1] = '\0';
    return dest;
}

InstType str_to_type(const char* str) {
    if (str_eq(str, "func")) {return FUNC;}

    else if (strlen(str) >= 1) {
        if (str[strlen(str)-1] == ':') {return LABEL;}
    }

    if (str_eq(str, "add")) { return ADD;}
    else if (str_eq(str, "sub")) { return SUB;}
    else if (str_eq(str, "div")) { return DIV;}
    else if (str_eq(str, "mod")) { return MOD;}
    else if (str_eq(str, "mult")) { return MULT;}
    else if (str_eq(str, "inc")) { return INC;}
    else if (str_eq(str, "dec")) { return DEC;}

    else if (str_eq(str, "push")) { return PUSH;}
    else if (str_eq(str, "push_lit")) { return PUSH_LIT;}
    else if (str_eq(str, "dupe")) { return DUPE;}
    else if (str_eq(str, "pop")) {return POP;}

    else if (str_eq(str, "stop")) { return STOP;}
    else if (str_eq(str, "dump")) { return DUMP;}
    else if (str_eq(str, "print")) { return PRINT;}
    else if (str_eq(str, "size")) { return SIZE;}
    else if (str_eq(str, "swap")) { return SWAP;}

    else if (str_eq(str, "mov")) {return MOV;}

    else if (str_eq(str, "jmp")) { return JMP;}
    else if (str_eq(str, "jmp_gt")) { return JMP_GT;}
    else if (str_eq(str, "jmp_gteq")) { return JMP_GTEQ;}
    else if (str_eq(str, "jmp_lt")) { return JMP_LT;}
    else if (str_eq(str, "jmp_lteq")) { return JMP_LTEQ;}
    else if (str_eq(str, "jmp_eq")) { return JMP_EQ;}
    else if (str_eq(str, "jmp_neq")) { return JMP_NEQ;}
    else if (str_eq(str, "cmp")) { return CMP;}

    else if (str_eq(str, "mem_read")) {return MEM_READ;}
    else if (str_eq(str, "mem_write")) {return MEM_WRITE;}
    else if (str_eq(str, "str")) {return STR;}
    else if (str_eq(str, "arr")) {return ARR;}

    else if (str_eq(str, "vid_mem_read")) {return VID_MEM_READ;}
    else if (str_eq(str, "vid_mem_write")) {return VID_MEM_WRITE;}
    else if (str_eq(str, "draw")) {return DRAW;}
    else if (str_eq(str, "clear_screen")) {return CLEAR_SCREEN;}

    else if (str_eq(str, "ret")) {return RET;}
    else if (str_eq(str, "call")) {return CALL;}

    else if (str_eq(str, "include")) {return INCLUDE;}
    
    else if (str_eq(str, "wait")) {return WAIT;}
    
    else if (str_eq(str, "")) { return EMPTY;}

    printf("Unknown Instruction: %s\n", str);

    return ILL;
}

Inst parse_line(char* line) {
    // remove_newline(line);
    Inst i = {0};
    i.type = EMPTY;
    i.has_operand = false;
    i.has_operator = false;
    i.has_literal = false;
    i.literal = NULL;
    i.line_number = 0;

    if (line[0] == ';') {
        return i;
    }

    // We need a copy of the line
    size_t line_length = strlen(line);
    char* new_line = (char*)malloc(line_length+1);
    if (new_line != NULL) {
        for (size_t j = 0; j < line_length; ++j) {
            new_line[j] = line[j];
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
                    
            // The literal for each InstType has to be handled differently
            if (i.type == PUSH_LIT) {
                i.literal = substr(line, 10, strlen(line)-2);

                if (i.literal == NULL) {
                    i.type = ILL;
                    return i;
                }

                i.has_literal = true;
                i.was_literal_alloced = true;
            }

            if (i.type == INCLUDE) {
                i.literal = substr(operand, 1, strlen(operand)-1);

                if (i.literal == NULL) {
                    i.type = ILL;
                    return i;
                }

                i.has_literal = true;
                i.was_literal_alloced = true;
            }

            if (i.type == FUNC) {
                i.literal = substr(operand, 0, strlen(operand)-1);

                if (i.literal == NULL) {
                    i.type = ILL;
                    return i;
                }

                i.has_literal = true;
                i.was_literal_alloced = true;
            }

            if (i.type == STR) {
                i.literal = substr(line, 5, strlen(line)-2);
                i.has_literal = true;
                i.was_literal_alloced = true;
            }

            if ((i.type >= JMP && i.type <= JMP_NEQ) || (i.type == CALL)) {
                i.literal = substr(operand, 0, strlen(operand));

                if (i.literal == NULL) {
                    i.type = ILL;
                    return i;
                }

                i.has_literal = true;
                i.was_literal_alloced = true;
                i.has_operand = false;
            } else {
                // We can set the operand if we don't need to handle the literal
                if (!isnum(operand) && operand[0] != '-') {
                    i.operand = operand[0]; 
                } else {
                    i.operand = atoi(operand);
                }
                i.has_operand = true;
            }

            if (operand[0] == '.') {
                // Dot operator 
                i.literal = ".";
                i.has_literal = true;
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
                i.has_literal = true;
                i.has_operator = false;
            }
        } else {
            break;
        }

        token = strtok(NULL, " ");
    }

    if (i.type == LABEL) {
        i.literal = substr(line, 0, strlen(line)-2);

        if (i.literal == NULL) {
            i.type = ILL;
            return i;
        }

        i.has_literal = true;
        i.was_literal_alloced = true;
    }

    free(new_line);
    return i;
}

#endif //__PARSER_H
