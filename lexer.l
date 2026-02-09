%{
#include "compiler.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap

/* Definitions */
KEYWORD           (PROGRAM|FUNCTION|VAR|INTEGER|STRING|WHILE|DO|IF|THEN|ELSE|BEGIN|END)
IDENTIFIER        [a-z]+
CONSTANT          (0|[+-]?[1-9][0-9]*)
STRING_LITERAL    '([a-zA-Z0-9 ]*)'
PUNCTUATOR        ("<="|">="|"<>"|":="|"*"|"/"|"+"|"-"|"<"|">"|"="|"("|")"|","|";"|"."|":")
COMMENT           \{[^\}\n]*\}
SPACE             [ \t\n\r]

/* Rules */
%%
{COMMENT}         {}
{SPACE}           {}
"PROGRAM"         {return PROGRAM;}
"FUNCTION"        {return FUNCTION;}
"VAR"             {return VAR;}
"INTEGER"         {return INTEGER;}
"STRING"          {return STRING;}
"WHILE"           {return WHILE;}
"DO"              {return DO;}
"IF"              {return IF;}
"THEN"            {return THEN;}
"ELSE"            {return ELSE;}
"BEGIN"           {return T_BEGIN;}
"END"             {return END;}
{IDENTIFIER}      {yylval.str = strdup(yytext);
                   return IDENTIFIER;}
{CONSTANT}        {yylval.num = atoi(yytext); return CONSTANT;}
{STRING_LITERAL}  {
    int len = strlen(yytext);
    char *s = (char*)malloc(len-1);
    strncpy(s, yytext+1, len-2);
    s[len-2]=  '\0';
    yylval.str = s;
    return STRING_LITERAL;
}
":="              {return OP_ASSIGN;}
"+"               {return OP_PLUS;}
"-"               {return OP_MINUS;}
"*"               {return OP_MULT;}
"/"               {return OP_DIV;}
"<="              {return OP_LEQ;}
">="              {return OP_GEQ;}
"<>"              {return OP_NEQ;}
"="               {return OP_EQ;}
"<"               {return OP_LT;}
">"               {return OP_GT;}
"("               {return '(';}
")"               {return ')';}
","               {return ',';}
";"               {return ';';}
"."               {return '.';}
":"               {return ':';}
.                 {return UNKNOWN;}

%%