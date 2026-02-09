#ifndef ASM_EMITTER_H
#define ASM_EMITTER_H

#include "quad.h"
#include "symbol_table.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

//compiler related code
class asmEmitter {
    public:
        vector<string> code;

        asmEmitter() : code() {};
        void genAssembly( map<string, vector<Quad*>>& funcQuads);
        
        void print(int num);

    private:
        void emitASM(const string& a);

        
        //Data segment
        void dataRegion(SymbolTable* globalST);
        void processGlobalsAndLiterals(SymbolTable* st);
        void emitGlobalVar(Symbol* sym);
        void emitLiteral(Symbol* sym);
        
        //Text segment
        void textRegion(vector<SymbolTable*>& TableList, map<string, vector<Quad*>>& funcQuads);
        void processFunc(SymbolTable* funcST, vector<Quad*>& quads);
        void emitFuncPrologue(SymbolTable* funcST);
        void emitFuncBody(vector<Quad*>& quads,  SymbolTable* funcST);
        void emitFuncEpilogue(const string& funcName);

        //Instruction handlers
        void handleAssign(Quad* q, SymbolTable* funcST);
        void handleArithmetic(Quad* q, SymbolTable* funcST);
        void handleConditional(Quad* q, SymbolTable* funcST);
        void handleGoto(Quad* q, SymbolTable* funcST);
        void handleParam(Quad* q, SymbolTable* funcST, int paramIndex);
        void handleCall(Quad* q, SymbolTable* funcST);
        void handleReturn(Quad* q, SymbolTable* funcST);

        // Utility functions
        string getSymbolAddress(Symbol* s);
        string getLiteralLabel(Symbol* s);
        bool isLiteral(const string& name);
        string formatStackAddr(int offset);
};


#endif