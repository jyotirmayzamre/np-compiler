#ifndef QUAD_H
#define QUAD_H

#include "opcodes.h"
#include <string>

using namespace std;

class Quad{
    public:
        Opcode op;
        string arg1;
        string arg2;
        string result;

        Quad(Opcode op, const string& arg1, const string& arg2, const string& result)
        : op(op), arg1(arg1), arg2(arg2), result(result) {}       
};

#endif