%{
#include <stdio.h>
#include <stdlib.h>
#include "Jyotirmay_Zamre_PA4_compiler.h"
int yylex(void);
void yyerror(const char *s);
%}

%union{
    Symbol* sym;
    int num;
    char* str;
    SymbolList* symList;
    BoolList* bList;
    StatementList* sList;
    Type* type;
}


/* Token Declarations */
%token PROGRAM FUNCTION VAR INTEGER STRING WHILE DO IF THEN ELSE T_BEGIN END
%token OP_ASSIGN OP_PLUS OP_MINUS OP_MULT OP_DIV OP_LEQ OP_GEQ OP_NEQ OP_EQ OP_LT OP_GT
%token <str> IDENTIFIER
%token <num> CONSTANT
%token <str> STRING_LITERAL
%token UNKNOWN

%type <sym> ARITHMETIC_EXPRESSION
%type <bList> BOOLEAN_EXPRESSION
%type <symList> EXPRESSION_LIST
%type <symList> VAR_LIST
%type <symList> PARAM_LIST
%type <symList> DECLARATION_LIST
%type <sList> SELECTION_STATEMENT
%type <sList> ITERATION_STATEMENT
%type <sList> COMPOUND_STATEMENT
%type <sList> STATEMENT_LIST
%type <sList> STATEMENT
%type <sym> SIMPLE_EXPRESSION
%type <sym> TERM
%type <sym> FACTOR
%type <type> TYPE
%type <num> M
%type <sList> N

%define parse.error verbose

%start PROG

/* Grammar Rules */
%%
PROG:
    PROGRAM IDENTIFIER ';' DECLARATION_LIST_OPT FUNCTION_DECLARATION_LIST_OPT COMPOUND_STATEMENT '.' {
        GlobalQA->emit(Opcode::OP_STOP, "", "", "");
    };

DECLARATION_LIST_OPT:
    DECLARATION_LIST | ;

DECLARATION_LIST:
    DECLARATION_LIST VAR VAR_LIST ':' TYPE ';' {
        string scope = currST->name == "main" ? "global" : "local";
        for (auto& var: $3->arr){
            var->update($5->type, scope);
        };
        $$ = $1;
        $$->arr = mergeLists($$->arr, $3->arr);
    }
    | VAR VAR_LIST ':' TYPE ';' {
        string scope = currST->name == "main" ? "global" : "local";
        for (auto& var: $2->arr){
            var->update($4->type, scope);
        };
        $$ = $2;
    };

FUNCTION_DECLARATION_LIST_OPT:
    FUNCTION_DECLARATION_LIST | ;

FUNCTION_DECLARATION_LIST:
    FUNCTION_DECLARATION_LIST FUNCTION_DECLARATION ';' | FUNCTION_DECLARATION ';' ;


FUNCTION_DECLARATION:
    FUNCTION_HEADER {
        GlobalQA->emit(Opcode::OP_FUNCTION, currST->name);
    } 
    DECLARATION_LIST_OPT {
        Symbol* retval = currST->insert("__retval");
        Symbol* funcSym = currST->parent->lookup(currST->name);

        string returnType = extractReturnType(funcSym->type);
        retval->update(returnType, "return");
    }
    COMPOUND_STATEMENT {
        backpatch($5->nextlist, quadIndex);
        GlobalQA->emit(Opcode::OP_RETURN, "__retval");
        currST = currST->parent;
    };


/*Update the function symbols type, size, offset, etc and create new symbol table for function*/
FUNCTION_HEADER:
    FUNCTION IDENTIFIER {
        Symbol* funcSym = currST->insert($2);
        SymbolTable* newTable = new SymbolTable(funcSym->name, currST);
        funcSym->update("temp", "func", newTable);
        TableList.push_back(newTable);
        currST = newTable;
        $<sym>$ = funcSym;
    }
    '(' PARAM_LIST ')' ':' TYPE ';' {
        string func_type = "";
        size_t len = $5->arr.size();
        for (size_t i = 0; i < len; i++) {
            auto& param = $5->arr[i];
            func_type += param->type + (i == len - 1 ? " -> " : " x ");
        };
        func_type += $8->type;
        $<sym>3->update(func_type); 
    };


PARAM_LIST:
    VAR_LIST ':' TYPE {
        for(auto& var : $1->arr){
            var->update($3->type, "param");
        };
        $$ = $1;
    } 
    
    | PARAM_LIST ';' VAR_LIST ':' TYPE {
        for(auto& var : $3->arr){
            var->update($5->type, "param");
        };
        $$ = $1;
        $$->arr = mergeLists($$->arr, $3->arr);
    };

VAR_LIST:
    IDENTIFIER {
        $$ = new SymbolList();
        Symbol* sym = currST->insert($1);
        $$->arr.push_back(sym);
    } | VAR_LIST ',' IDENTIFIER {
        $$ = $1;
        Symbol* sym = currST->insert($3);
        $$->arr.push_back(sym);
    };

TYPE:
    INTEGER {
        $$ = new Type();
        $$->type = "INTEGER";
    }| STRING {
        $$ = new Type();
        $$->type = "STRING";
    };

STATEMENT_LIST:
    STATEMENT {
        $$ = $1;
    }
    | STATEMENT_LIST M ';' STATEMENT{
        backpatch($1->nextlist, $2);
        $$ = $4;
    };

STATEMENT:
    IDENTIFIER OP_ASSIGN ARITHMETIC_EXPRESSION {
        Symbol* sym = currST->lookup($1);
        if(currST->parent != nullptr && currST->name == sym->name){
            Symbol* retval = currST->lookup("__retval");
            GlobalQA->emit(Opcode::OP_ASSIGN, $3->name, retval->name);
        } else {
            GlobalQA->emit(Opcode::OP_ASSIGN, $3->name, sym->name);
        }
        $$ = new StatementList();
    }

    | COMPOUND_STATEMENT {
        $$ = $1;
    }
    | SELECTION_STATEMENT {
        $$ = $1;
    }
    | ITERATION_STATEMENT {
        $$ = $1;
    };

COMPOUND_STATEMENT:
    T_BEGIN STATEMENT_LIST M END {
        backpatch($2->nextlist, $3);
        $$ = $2;
    };

SELECTION_STATEMENT:
    IF BOOLEAN_EXPRESSION THEN M STATEMENT {
        $$ = new StatementList();
        backpatch($2->truelist, $4);
        $$->nextlist = mergeLists($2->falselist, $5->nextlist);
    }
    | IF BOOLEAN_EXPRESSION THEN M STATEMENT ELSE N M STATEMENT {
        $$ = new StatementList();
        backpatch($2->truelist, $4);
        backpatch($2->falselist, $8);
        $$->nextlist = mergeLists($5->nextlist, mergeLists($7->nextlist, $9->nextlist));
    };

M: {
    $$ = quadIndex;
};

N: {
    $$ = new StatementList();
    $$->nextlist = makeList(quadIndex);
    GlobalQA->emit(Opcode::OP_GOTO, "", "", "");
};

ITERATION_STATEMENT:
    WHILE M BOOLEAN_EXPRESSION M DO STATEMENT {
        $$ = new StatementList();
        backpatch($3->truelist, $4);
        backpatch($6->nextlist, $2);
        $$->nextlist = $3->falselist;
        GlobalQA->emit(Opcode::OP_GOTO, "", "", to_string($2));
    };


ARITHMETIC_EXPRESSION:
    SIMPLE_EXPRESSION {
        $$ = $1;
    };

BOOLEAN_EXPRESSION:
    SIMPLE_EXPRESSION OP_LT SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_LT, $1->name, $3->name);
    }
    | SIMPLE_EXPRESSION OP_GT SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_GT, $1->name, $3->name);
    }
    | SIMPLE_EXPRESSION OP_LEQ SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_LEQ, $1->name, $3->name);
    }
    | SIMPLE_EXPRESSION OP_GEQ SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_GEQ, $1->name, $3->name);
    }
    | SIMPLE_EXPRESSION OP_EQ SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_EQ, $1->name, $3->name);
    }
    | SIMPLE_EXPRESSION OP_NEQ SIMPLE_EXPRESSION {
        $$ = parseBoolExpr(Opcode::OP_NEQ, $1->name, $3->name);
    };


SIMPLE_EXPRESSION:
    TERM {
        $$ = $1;
    } 
    
    | SIMPLE_EXPRESSION OP_PLUS TERM {
        $$ = parseArithExpr($1, $3, Opcode::OP_PLUS);        
    }

    | SIMPLE_EXPRESSION OP_MINUS TERM {
        $$ = parseArithExpr($1, $3, Opcode::OP_MINUS);
    };

TERM:
    FACTOR {
        $$ = $1;
    } 
    
    | TERM OP_MULT FACTOR {
        $$ = parseArithExpr($1, $3, Opcode::OP_MULT);
    } 
    
    | TERM OP_DIV FACTOR {
        $$ = parseArithExpr($1, $3, Opcode::OP_DIV);
    };

FACTOR:
    IDENTIFIER {
        $$ = currST->lookup($1);
    }

    | CONSTANT {
        $$ = GlobalST->insertLiteral(to_string($1), "INTEGER");        
    } 

    | STRING_LITERAL {
        $$ = GlobalST->insertLiteral($1, "STRING");
    } 
    
    | '(' ARITHMETIC_EXPRESSION ')' {
        $$ = $2;
    }
    
    | IDENTIFIER '(' EXPRESSION_LIST ')' {
        Symbol* sym = currST->lookup($1);
        $$ = currST->gentemp();
        for(auto x : $3->arr){
            GlobalQA->emit(Opcode::OP_PARAM, x->name);
        };

        string returnType = extractReturnType(sym->type);
        $$->update(returnType, "temp");
        GlobalQA->emit(Opcode::OP_CALL, sym->name, to_string($3->arr.size()), $$->name);
    };


EXPRESSION_LIST:
    ARITHMETIC_EXPRESSION {
        $$ = new SymbolList();
        $$->arr.push_back($1);
    } 
    
    | EXPRESSION_LIST ',' ARITHMETIC_EXPRESSION {
        $$ = $1;
        $$->arr.push_back($3);
    };

%%

void yyerror(const char *s) {
    printf("%s\n", s);
}

