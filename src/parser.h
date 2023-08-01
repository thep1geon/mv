#ifndef __PARSER_H
#define __PARSER_H

#include "err.h"
#include "etypes.h"
#include "inst.h"
#include "stack.h"
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
        return 0;  // Empty string or NULL pointer
    }

    while (*str) {
        if (!isdigit(*str)) {
            return false;  // Non-digit character found
        }
        str++;
    }

    return false;  // All characters are digits
}

Inst_Type str_to_type(const char* str) {
    if (strcmp(str, "add") == 0) { return ADD;}
    else if (strcmp(str, "sub") == 0) { return SUB;}
    else if (strcmp(str, "div") == 0) { return DIV;}
    else if (strcmp(str, "mult") == 0) { return MULT;}
    else if (strcmp(str, "inc") == 0) { return INC;}
    else if (strcmp(str, "dec") == 0) { return DEC;}

    else if (strcmp(str, "push") == 0) { return PUSH;}
    else if (strcmp(str, "dupe") == 0) { return DUPE;}
    else if (strcmp(str, "pop") == 0) {return POP;}

    else if (strcmp(str, "stop") == 0) { return STOP;}
    else if (strcmp(str, "dump") == 0) { return DUMP;}

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
    i.has_operand = false;
    i.has_operator = false;

    char* token = strtok(line, " ");
    char* type = NULL;
    char* operand = NULL;
    char* operator = NULL;

    while (token != NULL) {
        if (type == NULL) {
            type = token;
            remove_newline(type);
        } else if (operand == NULL) {
            operand = token;
            remove_newline(operand);
        } else if (operator == NULL) {
            operator = token;
            remove_newline(operator);
        }
        token = strtok(NULL, " ");
    }

    if (operand != NULL) {
        i.operand = atoi(operand);
        i.has_operand = true;
    }

    if (operator != NULL) {
        i.operator = atoi(operator);
        i.has_operator = true;
    }

    if (type != NULL) {
        i.type = str_to_type(type);
    }

    
    return i;
}

#endif //__PARSER_H
