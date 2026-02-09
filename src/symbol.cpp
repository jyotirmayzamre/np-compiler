#include "symbol.h"
#include "types.h"

void Symbol::update(const string& type, const string& scope){
    unsigned int size;
    switch(getTypeKind(type)){
        case INT_T:    
            size = size_of_int;
            break;

        case STRING_T: 
            size = size_of_str;
            break;

        default:       
            size = 0;
            break;
    }

    this->type = type;
    this->scope = scope;
    this->size = size;
}

void Symbol::update(const string& type, const string& scope, SymbolTable* nestedTable){
    unsigned int size;
    switch(getTypeKind(type)){
        case INT_T:    
            size = size_of_int;
            break;
        case STRING_T: 
            size = size_of_str;
            break;
        default:       
            size = 0;
            break;
    }

    this->type = type;
    this->scope = scope;
    this->size = size;
    this->nestedTable = nestedTable;
}

void Symbol::update(const string& type){
    this->type = type;
}

void Symbol::update(int offset){
    this->offset = offset;
}

void Symbol::update(const string& type, const string& scope, const string& initValue){
    unsigned int size;
    switch(getTypeKind(type)){
        case INT_T:    
            size = size_of_int;
            break;

        case STRING_T: 
            size = size_of_str;
            break;

        default:       
            size = 0;
            break;
    }

    this->type = type;
    this->scope = scope;
    this->size = size;
    this->initValue = initValue;
}
