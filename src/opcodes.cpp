#include "opcodes.h"

string opcodeToString(Opcode op) {
    switch (op) {
        case Opcode::OP_PLUS:     return "+";
        case Opcode::OP_MINUS:    return "-";
        case Opcode::OP_MULT:     return "*";
        case Opcode::OP_DIV:      return "/";
        case Opcode::OP_LT:       return "<";
        case Opcode::OP_GT:       return ">";
        case Opcode::OP_LEQ:      return "<=";
        case Opcode::OP_GEQ:      return ">=";
        case Opcode::OP_EQ:       return "=";
        case Opcode::OP_NEQ:      return "<>";
        case Opcode::OP_ASSIGN:   return ":=";
        case Opcode::OP_PARAM:    return "param";
        case Opcode::OP_CALL:     return "call";
        case Opcode::OP_GOTO:     return "goto";
        case Opcode::OP_RETURN:   return "return";
        case Opcode::OP_FUNCTION: return "function";
        case Opcode::OP_STOP:     return "stop";
    }
    return "UNKNOWN_OPCODE";
}