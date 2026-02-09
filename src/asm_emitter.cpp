#include "asm_emitter.h"
#include <iostream>

using namespace std;

//Public functions
void asmEmitter::genAssembly(map<string, vector<Quad*>>& funcQuads){
    dataRegion(GlobalST);
    textRegion(TableList, funcQuads);
}

void asmEmitter::print(int num){
    ofstream File("Asm/a" + to_string(num) + ".asm");
    File << left;
    for (auto& line : code){
        File << line << "\n";
    }
    File.close();
}



//Data Region
void asmEmitter::emitASM(const string& a){
    code.push_back(a);
}

void asmEmitter::dataRegion(SymbolTable* GlobalST){
    emitASM(".data");
    processGlobalsAndLiterals(GlobalST);
    emitASM("");
}

void asmEmitter::processGlobalsAndLiterals(SymbolTable* st) {
    emitASM("# Global Variables, Constants and Literals");
    
    for (auto& sym : st->table) {
        if (sym->scope == "func" || sym->scope == "temp") {
            continue;
        }

        if(sym->isLiteral){
            emitLiteral(sym);
            continue;
        }

        if (sym->scope == "global") {
            emitGlobalVar(sym);
            continue;
        }

    }
    
    emitASM("");
}

void asmEmitter::emitGlobalVar(Symbol* sym) {
    string label = sym->name + ":";

    if(sym->initValue.empty()){
        emitASM(label + " .quad 0");
        return;
    }

    if(sym->type == "INTEGER"){
        emitASM(label + " .quad " + sym->initValue);
        return;
    }

    if(sym->type == "STRING"){
        emitASM(label + " .asciz \"" + sym->initValue + "\"");
    }
}

void asmEmitter::emitLiteral(Symbol* sym) {
    string label = sym->name + ":";
    
    if (sym->type == "INTEGER") {
        emitASM(label + " .quad " + sym->initValue);
        return;
    } 
    
    if (sym->type == "STRING") {
        emitASM(label + " .asciz \"" + sym->initValue + "\"");
        return;
    }
}



//Text segment
void asmEmitter::textRegion(vector<SymbolTable*>& TableList, map<string, vector<Quad*>>& funcQuads){
    emitASM(".text");
    emitASM(".globl main");
    emitASM("");

    for (auto& table : TableList) {
        assignOffsets(table); 
        processFunc(table, funcQuads[table->name]);
    }
}


void asmEmitter::processFunc(SymbolTable* funcST, vector<Quad*>& quads){
    emitFuncPrologue(funcST);
    emitFuncBody(quads, funcST);
    emitFuncEpilogue(funcST->name);
}


void asmEmitter::emitFuncPrologue(SymbolTable* funcST){
    emitASM(funcST->name + ":");
    emitASM("   push %rbp");
    emitASM("   mov %rsp, %rbp");

    if(funcST->frameSize > 0){
        emitASM("   sub $" + to_string(funcST->frameSize) + ", %rsp");
    }

    emitASM("");
}


void asmEmitter::emitFuncBody(vector<Quad*>& quads, SymbolTable* funcST){    
    int paramIndex = 0;
    for(size_t i = 0; i < quads.size(); i++){
        Quad* q = quads[i];

        emitASM(funcST->name + "_L" + to_string(i) + ":");

        if(q->op == Opcode::OP_STOP){
            continue;
        }

        if(q->op == Opcode::OP_ASSIGN){
            handleAssign(q, funcST);
            continue;
        }

        if(q->op == Opcode::OP_PLUS || q->op == Opcode::OP_MINUS 
        || q->op == Opcode::OP_MULT || q->op == Opcode::OP_DIV){
            handleArithmetic(q, funcST);
            continue;
        }

        if(q->op == Opcode::OP_LT  || q->op == Opcode::OP_GT
        || q->op == Opcode::OP_LEQ || q->op == Opcode::OP_GEQ
        || q->op == Opcode::OP_EQ  || q->op == Opcode::OP_NEQ){
            handleConditional(q, funcST);
            continue;
        }

        if(q->op == Opcode::OP_GOTO){
            handleGoto(q, funcST);
            continue;
        }

        if(q->op == Opcode::OP_PARAM){
            handleParam(q, funcST, paramIndex++);
            continue;
        }

        if(q->op == Opcode::OP_CALL){
            handleCall(q, funcST);
            paramIndex = 0;
            continue;
        }

        if(q->op == Opcode::OP_RETURN){
            handleReturn(q, funcST);
            continue;
        }

    }
    emitASM("");
}



void asmEmitter::emitFuncEpilogue(const string& funcName){
    emitASM(funcName + "_epilogue: ");
    emitASM("   mov %rbp, %rsp");
    emitASM("   pop %rbp");
    if(funcName == "main"){
        emitASM("   mov $60, %rax");
        emitASM("   mov $0, %rdi");
        emitASM("   syscall");
    }else{
        emitASM("   ret");
    }
    emitASM("");
}

//Instruction Handlers

void asmEmitter::handleAssign(Quad* q, SymbolTable* funcST){
    Symbol* src = funcST->lookup(q->arg1);
    Symbol* dest = funcST->lookup(q->result);

    if(!src || !dest) return;

    string srcAddr = getSymbolAddress(src);
    string destAddr = getSymbolAddress(dest);

    bool isIntegerLiteral = src->isLiteral && src->type == "INTEGER";

    if (isIntegerLiteral) {
        emitASM("    mov " + srcAddr + ", %rax");
    } else if (src->isLiteral) {
        emitASM("    lea " + srcAddr + ", %rax");
    } else {
        emitASM("    mov " + srcAddr + ", %rax");
    }
    
    emitASM("    mov %rax, " + destAddr);
}

void asmEmitter::handleArithmetic(Quad* q, SymbolTable* funcST){
    Symbol* arg1 = funcST->lookup(q->arg1);
    Symbol* arg2 = funcST->lookup(q->arg2);
    Symbol* res  = funcST->lookup(q->result);

    string a1 = getSymbolAddress(arg1);
    string a2 = getSymbolAddress(arg2);
    string r  = getSymbolAddress(res);

     if(arg1->isLiteral){
        emitASM("    mov " + a1 + ", %rax");
    } else {
        emitASM("    mov " + a1 + ", %rax");
    }

   switch(q->op){
        case Opcode::OP_PLUS: {
            emitASM("   add " + a2 + ", %rax");
            break; 
        }
        
        case Opcode::OP_MINUS: {
            emitASM("    sub " + a2 + ", %rax");
            break;
        }

        case Opcode::OP_MULT: {
            emitASM("    imul " + a2 + ", %rax");
            break;
        }

        case Opcode::OP_DIV: {
            emitASM("    cqo");
            if(arg2->isLiteral){
                emitASM("    mov " + a2 + ", %rbx");
                emitASM("    idiv %rbx");
            } else {
                emitASM("    idiv " + a2);
            }
            break;
        }

        default: break;
    }

    emitASM("    mov %rax, " + r);
}

void asmEmitter::handleConditional(Quad* q, SymbolTable* funcST){
    Symbol* arg1 = funcST->lookup(q->arg1);
    Symbol* arg2 = funcST->lookup(q->arg2);

    if(!arg1 || !arg2) return;

    string a1 = getSymbolAddress(arg1);
    string a2 = getSymbolAddress(arg2);

    
    if(arg1->isLiteral){
        emitASM("    mov " + a1 + ", %rax");
    }

    if(arg2->isLiteral){
        emitASM("    cmp " + a2 + ", %rax");
    }

    string label = funcST->name + "_L" + q->result;

    switch(q->op){
        case Opcode::OP_LT: {
            emitASM("    jl " + label);
            break;
        }

        case Opcode::OP_GT: {
            emitASM("    jg " + label);
            break;
        }

        case Opcode::OP_LEQ: {
            emitASM("    jle " + label);
            break;
        }

        case Opcode::OP_GEQ: {
            emitASM("    jge " + label);
            break;
        }

        case Opcode::OP_EQ: {
            emitASM("    je " + label);
            break;
        }

        case Opcode::OP_NEQ: {
            emitASM("    jne " + label);
            break;
        }

        default:
            break;
    }
}

void asmEmitter::handleGoto(Quad* q, SymbolTable* funcST){
    emitASM("   jmp " + funcST->name + "_L" +  q->result);
}


void asmEmitter::handleParam(Quad* q, SymbolTable* funcST, int paramIndex){
    Symbol* sym = funcST->lookup(q->arg1);
    if(!sym) return;

    vector<string> paramRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    string srcAddr = getSymbolAddress(sym);

    bool useRegister = paramIndex < (int)paramRegs.size();
    bool isIntegerLiteral = sym->isLiteral && sym->type == "INTEGER";

    if (useRegister) {
        std::string reg = paramRegs[paramIndex];
        std::string instr = (sym->isLiteral && !isIntegerLiteral) ? "lea" : "mov";
        emitASM("    " + instr + " " + srcAddr + ", " + reg);

    } else {
        if (sym->isLiteral)
            emitASM("    push " + srcAddr + "(%rip)");
        else
            emitASM("    push " + srcAddr);
    }
}



void asmEmitter::handleCall(Quad* q, SymbolTable* funcST){
    int numParams = stoi(q->arg2);
    Symbol* ret = funcST->lookup(q->result);

    emitASM("    call " + q->arg1);

    if(numParams > 6){
        int pushed = numParams - 6;
        emitASM("    add $" + to_string(8 * pushed) + ", %rsp");
    }

    if(ret){
        emitASM("    mov %rax, " + getSymbolAddress(ret));
    }
}


void asmEmitter::handleReturn(Quad* q, SymbolTable* funcST){
    Symbol* ret = funcST->lookup(q->arg1);
    if(!ret) return;
    
    string retAddr = getSymbolAddress(ret);
    if(ret->isLiteral){
        emitASM("    mov " + retAddr + ", %rax");
    }
    emitASM("    jmp " + funcST->name + "_epilogue");
}



//Utility functions
string asmEmitter::getLiteralLabel(Symbol* s) {
    if (s->isLiteral) {
        return s->name;  
    }
    return "";
}

bool asmEmitter::isLiteral(const string& name) {
    return name.substr(0, 5) == "_lit_";
}


string asmEmitter::formatStackAddr(int offset) {
    if (offset < 0) {
        return to_string(offset) + "(%rbp)";
    } else {
        return to_string(offset) + "(%rbp)";
    }
}


string asmEmitter::getSymbolAddress(Symbol* sym){
    if(!sym) return "0"; 

    if(sym->scope == "param"){
        int paramIndex = (sym->offset - 16) / 8;
        vector<string> paramRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        
        if(paramIndex < (int)paramRegs.size()){
            return paramRegs[paramIndex];  
        } else {
            return formatStackAddr(sym->offset);
        }
    }
    
    if(sym->scope == "local" || sym->scope == "temp" || sym->scope == "return"){
        return formatStackAddr(sym->offset);
    }

    return sym->name + "(%rip)";

}
