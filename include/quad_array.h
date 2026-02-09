#ifndef QUAD_ARRAY_H
#define QUAD_ARRAY_H

#include "quad.h"
#include "symbol.h"
#include "types.h"
#include <vector>
#include <map>
#include <unordered_set>

using namespace std;

class QuadArray {
    public:
        vector<Quad*> arr;

        QuadArray() : arr() {};

        unsigned int emit(Opcode op, const string& arg1, const string& arg2, const string& result);
        unsigned int emit(Opcode op, const string& arg1);
        unsigned int emit(Opcode op, const string& arg1, const string& result);



        void print(int num);       
};

extern unsigned int quadIndex;
extern QuadArray* GlobalQA;

void backpatch(vector<unsigned int>& arr, const unsigned int target);

vector<unsigned int> mergeLists(const vector<unsigned int>& L1, const vector<unsigned int>& L2);
vector<Symbol*> mergeLists(const vector<Symbol*>& L1, const vector<Symbol*>& L2);

vector<unsigned int> makeList(unsigned int i);
vector<Symbol*> makeList(Symbol* sym);



BoolList* parseBoolExpr(Opcode op, const string& arg1, const string& arg2);
Symbol* parseArithExpr(Symbol* term1, Symbol* term2, Opcode op);


void separateQuads(QuadArray* GlobalQA, map<string, vector<Quad*>>& funcQuads);

#endif