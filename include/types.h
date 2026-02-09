#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

class Symbol;

// Constants
const unsigned int size_of_int = 8;
const unsigned int size_of_str = 8;

enum TypeKind { INT_T, STRING_T, UNKNOWN_T };
TypeKind getTypeKind(const std::string& type);
std::string extractReturnType(const std::string& typeStr);

struct Type {
    std::string type;
};

struct SymbolList {
    std::vector<Symbol*> arr;
};

struct BoolList {
    std::vector<unsigned int> truelist;
    std::vector<unsigned int> falselist;
};

struct StatementList {
    std::vector<unsigned int> nextlist;
};

#endif