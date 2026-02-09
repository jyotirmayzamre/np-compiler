#include "types.h"
#include "symbol.h"
#include "quad_array.h"
#include "symbol_table.h"
#include <string>

using namespace std;

extern QuadArray* GlobalQA;
extern SymbolTable* currST;
extern vector<SymbolTable*> TableList;

TypeKind getTypeKind(const std::string& type) {
    if (type == "INTEGER") return INT_T;
    if (type == "STRING") return STRING_T;
    return UNKNOWN_T;
}


void backpatch(vector<unsigned int>& arr, const unsigned int target){
    string label = to_string(target);
    for (auto& i : arr){
        GlobalQA->arr[i]->result = label;
    }
}

vector<unsigned int> makeList(unsigned int i){
    vector<unsigned int> L = {i};
    return L;
}

vector<Symbol*> makeList(Symbol* sym){
    vector<Symbol*> L = {sym};
    return L;
}

vector<unsigned int> mergeLists(const vector<unsigned int>& L1, const vector<unsigned int>& L2){
    vector<unsigned int> merged = L1;  
    merged.insert(merged.end(), L2.begin(), L2.end()); 
    return merged;
}

vector<Symbol*> mergeLists(const vector<Symbol*>& L1, const vector<Symbol*>& L2){
    vector<Symbol*> merged = L1;  
    merged.insert(merged.end(), L2.begin(), L2.end()); 
    return merged;
}

BoolList* parseBoolExpr(Opcode op, const string& arg1, const string& arg2){
    BoolList* newList = new BoolList();
    newList->truelist = makeList(GlobalQA->emit(op, arg1, arg2, ""));
    newList->falselist = makeList(GlobalQA->emit(Opcode::OP_GOTO, "", "", ""));
    return newList;
}

Symbol* parseArithExpr(Symbol* term1, Symbol* term2, Opcode op){
    Symbol* curr;
    if(term1->scope == "temp"){
        curr = term1;
    } else if (term2->scope == "temp"){
        curr = term2;
    } else{
        curr = currST->gentemp();
        curr->update("INTEGER", "temp");
    }

    GlobalQA->emit(op, term1->name, term2->name, curr->name);
    return curr;
}

string extractReturnType(const string& typeStr) {
    std::string returnType = "INTEGER";

    size_t arrowPos = typeStr.find("->");
    if (arrowPos != std::string::npos) {
        returnType = typeStr.substr(arrowPos + 2);

        size_t start = returnType.find_first_not_of(" ");
        if (start != std::string::npos) {
            returnType = returnType.substr(start);
        }
    }

    return returnType;
}


void cleanup(map<string, vector<Quad*>>& funcQuads){
    for(auto& table: TableList){
        for(auto& sym : table->table){
            delete sym;
        }
        delete table;
    }

    for (auto& q : GlobalQA->arr){
        delete q;
    }

    delete GlobalQA;

    funcQuads.clear();
}