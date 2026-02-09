#include "quad_array.h"
#include "opcodes.h"
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

extern QuadArray* GlobalQA;
extern unsigned int quadIndex;

void QuadArray::print(int num){
    int i = 0;
    ofstream File("Quads/Jyotirmay_Zamre_PA4_quads" + to_string(num) + ".out");
    File << left;
    for (auto& quad : arr){
        File << setw(10) << (to_string(i) + ":") << "\t" 
             << setw(15) << opcodeToString(quad->op) 
             << setw(15) << quad->arg1 
             << setw(15) << quad->arg2 
             << setw(15) << quad->result << "\n";
        i++;
    }
    File.close();
}

unsigned int QuadArray::emit(Opcode op, const string& arg1, const string& arg2, const string& result){
    arr.push_back(new Quad(op, arg1, arg2, result));
    quadIndex++;
    return quadIndex-1;
}

unsigned int QuadArray::emit(Opcode op, const string& arg1){
    arr.push_back(new Quad(op, arg1, "", ""));
    quadIndex++;
    return quadIndex-1;
}

unsigned int QuadArray::emit(Opcode op, const string& arg1, const string& result){
    arr.push_back(new Quad(op, arg1, "", result));
    quadIndex++;
    return quadIndex-1;
}


void separateQuads(QuadArray* GlobalQA, map<string, vector<Quad*>>& funcQuads) {
    map<int, int> localIndexMap; 
    string currentFunc = "main";
    int localIndex = -1;          

    for (int i = 0; i < GlobalQA->arr.size(); i++) {
        Quad* q = GlobalQA->arr[i];

        if (q->op == Opcode::OP_FUNCTION) {
            currentFunc = q->arg1;
            funcQuads[currentFunc].clear();
            localIndexMap.clear();
            localIndex = -1;
            continue;
        }

        if (currentFunc.empty())
            continue;  

        localIndex++;
        localIndexMap[i] = localIndex;

        funcQuads[currentFunc].push_back(q);

        if (q->op == Opcode::OP_RETURN) {
            currentFunc = "main";
            localIndex = -1;
        }
    }

    static const unordered_set<Opcode> branchOps = {
        Opcode::OP_GOTO, Opcode::OP_LT, Opcode::OP_GT, Opcode::OP_LEQ,
        Opcode::OP_GEQ, Opcode::OP_EQ, Opcode::OP_NEQ
    };

    for (auto& [funcName, qVec] : funcQuads) {
        for (Quad* q : qVec) {
            if (!branchOps.count(q->op))
                continue;
            int oldTarget = stoi(q->result);
            q->result = to_string(localIndexMap[oldTarget]);
        }
    }
}