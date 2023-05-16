%{
    #include <stdio.h>
    #include <iostream>
    #include <string>

    #include "parser.tab.h"
    #include "util/AST.hpp"
    #include "util/MIPS.hpp"
    // #include "struct_functions.hpp"

    // Debug
    // #include "enum_token.hpp"
    
    using std::cout;
    using std::endl;

    void printError(std::string token);
    extern "C" int yylex();
    extern FILE *yyin;

%}

%option yylineno
%option noyywrap

numbers [0-9]+
variable [a-zA-Z][_a-zA-Z0-9]*
white_space [ \t\n]*
multi_comment [/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]

%%
{white_space}   {;}
"main"          {return MAIN;}
"int"           {return INT;}
"void"          {return VOID;}
"break"         {return BREAK;}
"do"            {return DO;}
"else"          {return ELSE;}
"if"            {return IF;}
"while"         {return WHILE;}
"return"        {return RETURN;}
"scanf"         {return READ_;}
"printf"        {return WRITE_;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"{"             {return LBRACE;}
"}"             {return RBRACE;}
"["             {return LSQUARE;}
"]"             {return RSQUARE;}
";"             {return SEMI;}
"<<"            {return SHL_OP;}
">>"            {return SHR_OP;}
"=="            {return EQ;}
"!="            {return NOTEQ;}
"<="            {return LTEQ;}
">="            {return GTEQ;}
"&&"            {return ANDAND;}
"||"            {return OROR;}
"//".*          {;}
{multi_comment} {;}
[/][*]          {yy_fatal_error("Unterminated Comment");}
"&"             {return AND_OP;}
"|"             {return OR_OP;}
"!"             {return NOT_OP;}
"<"             {return LT;}
">"             {return GT;}
","             {return COMMA;}
"="             {return ASSIGNOP;}
"+"             {return PLUSOP;}
"-"             {return MINUSOP;}
"*"             {return MUL_OP;}
"/"             {return DIV_OP;}
{numbers}       {yylval.value = atoi(yytext); return INT_NUM;}
{variable}      {yylval.name = new std::string(yytext); return ID;}
.               {;}
%%

void printError(std::string token) {
    cout << "Undefined input " << token << endl;
    return;
}

// Testing
/* int main (int argc, char const *argv[]) {
    if (argc == 1) {
        while(yylex());
    }

    else {
        FILE *myfile = fopen(argv[1], "r");

        if (!myfile) {
            cout << "Error, unable to open file" << endl;
            return -1;
        }

        yyin = myfile;

        while(yylex());
    }
} */