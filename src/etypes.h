#ifndef __ETYPES_H
#define __ETYPES_H


// Enum of error types
typedef enum {
    None = 0, 

    STACK_StackOverflow,
    STACK_StackUnderflow,
    STACK_NodeAllocFail,
    STACK_StackAllocFail,
    STACK_EmptyStack,
    STACK_EmptyStackPop,
    STACK_EmptyStackPeek,
    STACK_IndexOutofBounds,
    STACK_DestStackNotEmpty,
    STACK_NullStack,

    MV_DivisionByZero,
    MV_Stop,
    MV_FileOpenFail,
    MV_ProgramNotFound,
    MV_MemFail,

    INST_IllegalInstruction,
    INST_TooManyParameters,
    INST_MissingParameters,
    INST_LabelNotFound,
} ErrType;

const char* err_type_to_str(ErrType e) {
    switch (e) {
        case None:                    return "None";
        case STACK_StackOverflow:     return "StackOverflow";
        case STACK_StackUnderflow:    return "StackUnderflow";
        case STACK_NodeAllocFail:     return "NodeAllocFail";
        case STACK_StackAllocFail:    return "StackAllocFail";
        case STACK_EmptyStack:        return "EmptyStack";
        case STACK_EmptyStackPop:     return "EmptyStackPop";
        case STACK_EmptyStackPeek:    return "EmptyStackPeek";
        case STACK_IndexOutofBounds:  return "IndexOutofBounds";
        case STACK_DestStackNotEmpty: return "DestStackNotEmpty";
        case STACK_NullStack:         return "NullStack";
        case MV_DivisionByZero:       return "DivisionByZero";
        case MV_Stop:                 return "Stop";
        case MV_FileOpenFail:         return "FileOpenFail";
        case MV_ProgramNotFound:      return "ProgramNotFound";
        case MV_MemFail:              return "MemFail";
        case INST_IllegalInstruction: return "IllegalInstruction";
        case INST_TooManyParameters:  return "TooManyParameters";
        case INST_MissingParameters:  return "MissingParameters";
        case INST_LabelNotFound:      return "LabelNotFound";
        default:                      return "Unknown Error";
    }
}

#endif //__ETYPES_H
