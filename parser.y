%code requires{    
    #include <iostream>
    #include <string>
    #include <vector>
    #include "struct_functions.hpp"
    // #include "util/symbol_table.hpp"
    // #include "util/AST.cpp"
    // #include "util/symbol_table.cpp"
    // #include "util/MIPS.cpp"

    // #define YYDEBUG 1
}

%code top {
    extern "C" int yylex(void);
    extern "C" int yyparse(void);

    void yyerror(char const *s);
}

%code {
    SymbTable *sym_table = new SymbTable();
    MIPS *code = new MIPS();   

}

%union{
    std::string *name;
    unsigned int value;
    Node *node;
    std::vector<Node*> *node_vector;
}

%token INT MAIN VOID BREAK DO ELSE IF WHILE RETURN READ_ WRITE_

%token LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE SEMI AND_OP
%token OR_OP NOT_OP LT RT SHL_OP SHR_OP EQ NOTEQ LTEQ GTEQ ANDAND OROR
%token COMMA ASSIGNOP PLUSOP MINUSOP MUL_OP DIV_OP

%token <value> INT_NUM
%token <name> ID

%type <node_vector> id_list id_list_tail expression_list expr_list_tail
%type <node> addop expression expression_pre primary
%type <value> int_literal

%start prog

%%
prog            : BEGIN_ statement_list END_;

statement_list  : 
                | statement statement_list;

statement       : ID ASSIGNOP expression SEMICOLON 
                {
                    // Create AST Tree
                    Node *root = new Node();
                    Node *new_ID = new Node(*($1));

                    root->right = $3;
                    root->left = new_ID;

                    root->genCode(sym_table, code);
                };
                | READ_ LPAREN id_list RPAREN SEMICOLON 
                {
                    code->MIPS_read(sym_table, $3);
                };
                | WRITE_ LPAREN expression_list RPAREN SEMICOLON
                {
                    code->MIPS_write(sym_table, $3);
                };

id_list         : ID id_list_tail
                {
                    // Add to id_list vector
                    Node *new_id = new Node(*($1));
                    $$ = $2;
                    $$->push_back(new_id);
                };

id_list_tail    : 
                {
                    $$ = new std::vector<Node*>;
                }
                | COMMA ID id_list_tail
                {
                    // Add to id_list vector
                    Node *new_id = new Node(*($2));
                    $$ = $3;
                    $$->push_back(new_id);
                };

expression_list : expression expr_list_tail
                {
                    $$ = $2;
                    $$->push_back($1);
                };

expr_list_tail  : 
                {
                    $$ = new std::vector<Node*>;
                }
                | COMMA expression expr_list_tail
                {
                    $$ = $3;
                    $$->push_back($2);
                };

expression      : expression_pre primary
                {
                    // Construct Expression Node
                    if ($1 == nullptr) 
                        $$ = $2;

                    else {
                        $1->right = $2;
                        $$ = $1;

                    }
                };

expression_pre : expression_pre primary addop 
                {
                    if ($1 == nullptr) 
                        $3->left = $2;

                    else {
                        $1->right = $2;
                        $3->left = $1;
                    }

                    $$ = $3;
                }
                | 
                {
                    $$ = nullptr;
                };

primary         : ID
                {
                    // Create ID
                    $$ = new Node(*($1));
                }
                | int_literal
                {
                    // Create INTLITERAL
                    $$ = new Node($1);
                }
                | LPAREN expression RPAREN
                {
                    $$ = $2;
                };

int_literal     : INTLITERAL
                {
                    $$ = $1;
                }
                | MINUSOP INTLITERAL
                {
                    $$ = -($2);
                }
                ;

addop           : PLUSOP
                {
                    // Create Operation Node
                    $$ = new Node(false);
                }
                | MINUSOP
                {
                    // Create Operation Node
                    $$ = new Node(true);
                    // std::cout << "minus op made" <<std::endl;
                };
%%

int main(int argc, char *argv[]) {

    // yydebug = 1;

    extern FILE *yyin;

    if (argc < 2) {
        yyparse();
        code->print();

        return 0;

    } else {
        yyin = fopen(argv[1], "r");

        yyparse();

        fclose(yyin);

        code->print();
    }
}

void yyerror(char const *s) {
    printf("%s\n", s);
}