#include "compiler.h"

SymbolTable* GlobalST = new SymbolTable("main", nullptr);
SymbolTable* currST = GlobalST;
unsigned int quadIndex = 0;
QuadArray* GlobalQA = new QuadArray();
vector<SymbolTable*> TableList = {GlobalST};
asmEmitter* ASM = new asmEmitter();