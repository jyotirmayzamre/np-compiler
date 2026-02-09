#include "symbol_table.h"
#include <iostream>
#include <iomanip>

using namespace std;

unsigned int SymbolTable::temp = 0;


Symbol* SymbolTable::lookup_local(const string& name){
    for(auto& sym: table){
        if(sym->name == name){
            return sym;
        }
    }
    return nullptr;
}

Symbol* SymbolTable::lookup(const string& name){
    Symbol* sym = lookup_local(name);
    if(sym == nullptr && parent != nullptr){
        sym = parent->lookup(name);
    }
    return sym;
}



Symbol* SymbolTable::insert(const string& name){
    Symbol* sym = lookup_local(name);
    if(sym != nullptr){
        return sym;
    } 
    sym = new Symbol(name);
    table.push_back(sym);
    return sym;
}



Symbol* SymbolTable::gentemp(){
    auto tempName = string("t") + to_string(temp);
    temp++;
    return insert(tempName);
}

Symbol* SymbolTable::insertLiteral(const string& value, const string& type){
    for(auto& sym: table){
        if(sym->isLiteral && sym->initValue == value && sym->type == type){
            return sym;
        }
    }
    
    static unsigned int literalCount = 0;
    string litName = "_lit_" + to_string(literalCount++);
    Symbol* sym = new Symbol(litName, type, "constant", 8, 0, nullptr, value, true);
    table.push_back(sym);
    return sym;
}


void SymbolTable::print(){
    string parentName = parent == nullptr ? "Null" : parent->name;
    cout << string(129, '-') << "\n";
    cout << "ST:" << name << ", Parent:" << parentName << "\n";
    cout << string(129, '-') << "\n";
    cout << left;
    cout << setw(15) << "Name" 
         << setw(60) << "Type" 
         << setw(10) << "Scope" 
         << setw(6)  << "Size" 
         << setw(8)  << "Offset" 
         << setw(10) << "initValue"
         << setw(10) << "isLiteral"
         << setw(10) << "Table" << "\n";


    for (auto &sym : table) {
        if(sym->scope == "temp") continue;
        string tableName = sym->nestedTable == nullptr ? "Null" : sym->nestedTable->name;
        cout      << setw(15) << sym->name
                  << setw(60) << sym->type
                  << setw(10) << sym->scope
                  << setw(6)  << sym->size
                  << setw(8)  << sym->offset
                  << setw(10) << sym->initValue
                  << setw(10) << (sym->isLiteral ? "Yes" : "No")
                  << setw(10) << tableName << "\n";
    }
    cout << string(129, '-') << "\n";
}

void assignOffsets(SymbolTable* st){
    int localOffset = 0;
    int paramOffset = 16;  

    for (auto& sym : st->table) {
        if (sym->scope == "param") {
            sym->offset = paramOffset;
            paramOffset += 8;  
            continue;
        }
        if (sym->scope == "local" || sym->scope == "temp" || sym->scope == "return") {
            localOffset += 8;
            sym->offset = -localOffset;
            continue;
        }
    }

    st->frameSize = localOffset;
    if (st->frameSize % 16 != 0) {
        st->frameSize = ((st->frameSize / 16) + 1) * 16;
    }
}


