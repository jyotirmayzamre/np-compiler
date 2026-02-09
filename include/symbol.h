#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

using namespace std;

class SymbolTable;

class Symbol{
    public:
        string name;
        string type;
        string scope;
        unsigned int size;
        int offset;
        SymbolTable* nestedTable;
        string initValue;
        bool isLiteral;


        Symbol(
            string name,
            string type = "",
            string scope = "",
            unsigned int size = 0,
            int offset = 0,
            SymbolTable* nestedTable = nullptr,
            string initValue = "",
            bool isLiteral = false
        ): name(name), type(type), scope(scope), size(size), offset(offset), nestedTable(nestedTable)
        , initValue(initValue), isLiteral(isLiteral) {};

        void update(const string& type, const string& scope);
        void update(const string& type, const string& scope, SymbolTable* nestedTable);
        void update(const string& type);
        void update(int pffset);
        void update(const string& type, const string& scope, const string& initVal);
        
};

#endif