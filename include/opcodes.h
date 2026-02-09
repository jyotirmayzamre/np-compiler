#ifndef OPCODES_H
#define OPCODES_H

#include <string>

using namespace std;

enum class Opcode {
    OP_PLUS, //+
    OP_MINUS, //-
    OP_MULT, //*
    OP_DIV, // /

    OP_LT, //<
    OP_GT, //>
    OP_LEQ, //<=
    OP_GEQ, //>=
    OP_EQ, //=
    OP_NEQ, //<>

    OP_ASSIGN, //:=
    
    OP_PARAM, //params
    OP_CALL, //function call
    OP_GOTO, //goto 
    OP_RETURN, //return
    OP_FUNCTION, //function labels
    OP_STOP
};

string opcodeToString(Opcode op);

#endif