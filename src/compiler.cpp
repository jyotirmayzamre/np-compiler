#include "compiler.h"

extern int yyparse();

extern SymbolTable* GlobalST;
extern SymbolTable* currST;
extern QuadArray* GlobalQA;
extern vector<SymbolTable*> TableList;
extern asmEmitter* ASM;
map<string, vector<Quad*>> funcQuads;

int main(int argc, char* argv[]){
    if(yyparse() == 0){

        //generate assembly
        separateQuads(GlobalQA, funcQuads);
        ASM->genAssembly(funcQuads);
        ASM->print(stoi(argv[1]));

        //print symbol tables
        for(auto& table : TableList){
            table->print();
        }
        
        //print global quadArray
        GlobalQA->print(stoi(argv[1]));
        

        cleanup(funcQuads);
        
    };
    return 0;
}