#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol.h"
#include <vector>
#include <string>

using namespace std;

class SymbolTable {
    public:
        string name;
        SymbolTable* parent;
        vector<Symbol*> table;
        //unsigned int currOffset;
        static unsigned int temp;
        unsigned int frameSize;

        SymbolTable(
            string name,
            SymbolTable* parent
        ) : name(name), parent(parent), table(), frameSize(0) {}; 

        Symbol* lookup(const string& name);
        Symbol* lookup_local(const string& name);
        Symbol* insert(const string& name);
        Symbol* gentemp();
        Symbol* insertLiteral(const string& value, const string& type);
        void print();
};

void assignOffsets(SymbolTable* st);
extern SymbolTable* GlobalST;
extern SymbolTable* currST;
extern vector<SymbolTable*> TableList;

#endif