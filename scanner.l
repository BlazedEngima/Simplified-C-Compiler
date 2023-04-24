%{
    #include <iostream>
    #include <string>

    // #include "parser.tab.h"
    // #include "util/AST.hpp"
    // #include "util/MIPS.hpp"
    // #include "struct_functions.hpp"

    // {numbers}     {yylval.value = atoi(yytext); cout << yylval.value << endl; return INT_NUM;}
    // {variable}    {yylval.name = new std::string(yytext); cout << yylval.name << endl; return ID;}

    // Debug
    #include "enum_token.hpp"
    
    using std::cout;
    using std::endl;

    void printError(std::string token);
    extern "C" int yylex();
    extern FILE *yyin;

%}

%option noyywrap

numbers [0-9]+
variable [a-zA-Z][a-zA-Z0-9]*
white_space [ \t\n]
multi_comment [/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]

/* {numbers}       {yylval.value = atoi(yytext); cout << yylval.value << endl; return INT_NUM;}
{variable}      {yylval.name = new std::string(yytext); cout << yylval.name << endl; return ID;} */

%%
"main"          {cout << "MAIN" << endl; return MAIN;}
"int"           {cout << "INT" << endl; return INT;}
"void"          {cout << "VOID" << endl; return VOID;}
"break"         {cout << "BREAK" << endl; return BREAK;}
"do"            {cout << "DO" << endl; return DO;}
"else"          {cout << "ELSE" << endl; return ELSE;}
"if"            {cout << "IF" << endl; return IF;}
"while"         {cout << "WHILE" << endl; return WHILE;}
"return"        {cout << "RETURN" << endl; return RETURN;}
"scanf"         {cout << "READ" << endl; return READ_;}
"printf"        {cout << "WRITE_" << endl; return WRITE_;}
"("             {cout << "LPAREN" << endl; return LPAREN;}
")"             {cout << "RPAREN" << endl; return RPAREN;}
"{"             {cout << "LBRACE" << endl; return LBRACE;}
"}"             {cout << "RBRACE" << endl; return RBRACE;}
"["             {cout << "LSQUARE" << endl; return LSQUARE;}
"]"             {cout << "RSQUARE" << endl; return RSQUARE;}
";"             {cout << "SEMI" << endl; return SEMI;}
"<<"            {cout << "SHL_OP" << endl; return SHL_OP;}
">>"            {cout << "SHR_OP" << endl; return SHR_OP;}
"=="            {cout << "EQ" << endl; return EQ;}
"!="            {cout << "NOTEQ" << endl; return NOTEQ;}
"<="            {cout << "LTEQ" << endl; return LTEQ;}
">="            {cout << "GTEQ" << endl; return GTEQ;}
"&&"            {cout << "ANDAND" << endl; return ANDAND;}
"||"            {cout << "OROR" << endl; return OROR;}
"//".*          {;}
{multi_comment} {;}
[/][*]          {yy_fatal_error("Unterminated Comment");}
"&"             {cout << "AND_OP" << endl; return AND_OP;}
"|"             {cout << "OR_OP" << endl; return OR_OP;}
"!"             {cout << "NOT_OP" << endl; return NOT_OP;}
"<"             {cout << "LT" << endl; return LT;}
">"             {cout << "RT" << endl; return RT;}
","             {cout << "COMMA" << endl; return COMMA;}
"="             {cout << "ASSIGNOP" << endl; return ASSIGNOP;}
"+"             {cout << "PLUSOP" << endl; return PLUSOP;}
"-"             {cout << "MINUSOP" << endl; return MINUSOP;}
"*"             {cout << "MUL_OP" << endl; return MUL_OP;}
"/"             {cout << "DIV_OP" << endl; return DIV_OP;}
{numbers}       {cout << "INT_NUM" << endl; return INT_NUM;}
{variable}      {cout << "ID" << endl; return ID;}
{white_space}   {;}
.               {printError(std::string(yytext));}
<<EOF>>         {cout << "EOF" << endl; return 0;}
%%

void printError(std::string token) {
    cout << "Undefined input " << token << endl;
    return;
}

// Testing
int main (int argc, char const *argv[]) {
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
}