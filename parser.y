%code requires{    
    #include <iostream>
    #include <string>
    #include <vector>
    #include <assert.h>
    #include "util/AST.hpp"
    #include "util/SymbTable.hpp"
    #include "util/MIPS.hpp"
}

%code top {
    extern "C" int yylex(void);
    extern "C" int yyparse(void);

    void yyerror(char const *s);
}

%code {
    SymbTable sym_table = SymbTable();
    std::vector<std::string> declared_id;
    MIPS mips = MIPS(&sym_table, &declared_id);

    int curr_scope = 0;
}

%union{
    std::string *name;
    int value;
    Node *node;
    std::vector<Node*> *node_vector;
}

%token INT MAIN VOID BREAK DO ELSE IF WHILE RETURN READ_ WRITE_
%token LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE SEMI AND_OP
%token OR_OP NOT_OP LT GT SHL_OP SHR_OP EQ NOTEQ LTEQ GTEQ ANDAND OROR
%token COMMA ASSIGNOP PLUSOP MINUSOP MUL_OP DIV_OP

%token <value> INT_NUM
%token <name> ID

%type <node_vector> declaration_list
%type <node> declaration
%type <node> exp exp1 exp2 exp3 exp4 exp5 exp6 exp7 exp8 exp9 exp10 exp11

%right THEN ELSE
%start prog

%%
prog                : var_declarations statements;

var_declarations    : var_declaration var_declarations;
                    | %empty ; 

var_declaration     : INT declaration_list SEMI 
                    {
                        for (auto it = $2->begin(); it != $2->end(); it++) {
                            (*it)->gen_declare_code(mips);
                        }

                        delete $2;
                    };

declaration_list    : declaration_list COMMA declaration 
                    {
                        $1->push_back($3);
                        $$ = $1;
                    };
                    | declaration 
                    {
                        $$ = new std::vector<Node*>;
                        $$->push_back($1);
                    };

declaration         : ID ASSIGNOP INT_NUM 
                    {
                        Node *id_node = new Node(_ID_, *($1));
                        Node *int_node = new Node(_INT_NUM_, $3);
                        $$ = new Node(_ROOT_);
                        $$->left = id_node;
                        $$->right = int_node;
                    };
                    | ID LSQUARE INT_NUM RSQUARE 
                    {
                        Node *id_node = new Node(_ID_, *($1));
                        Node *int_node = new Node(_INT_NUM_, $3);
                        $$ = new Node(_ARRAY_);
                        $$->left = id_node;
                        $$->right = int_node;
                    };
                    | ID 
                    {
                        $$ = new Node(_ID_, *($1));
                    };

code_block          : statement;
                    | LBRACE statements RBRACE;

statements          : statements statement;
                    | statement;

statement           : assign_statement SEMI;
                    | control_statement;
                    | read_write_statement SEMI;
                    | BREAK SEMI;
                    | SEMI;

control_statement   : if_statement;
                    | while_statement;
                    | do_while_statement SEMI;
                    | return_statement SEMI;

read_write_statement: read_statement;
                    | write_statement;

assign_statement    : ID LSQUARE exp RSQUARE ASSIGNOP exp;
                    | ID ASSIGNOP exp;

if_statement        : if_stmt %prec THEN;
                    | if_stmt ELSE code_block;

if_stmt             : IF LPAREN exp RPAREN code_block;

while_statement     : WHILE LPAREN exp RPAREN code_block;

do_while_statement  : DO code_block WHILE LPAREN exp RPAREN;

return_statement    : RETURN;

read_statement      : READ_ LPAREN ID RPAREN;

write_statement     : WRITE_ LPAREN exp RPAREN;

exp                 : exp1
                    {
                        $$ = $1;
                    };

exp1                : exp1 OROR exp2
                    {
                        $$ = new Node(_EXP_, "||");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp2
                    {
                        $$ = $1;
                    };

exp2                : exp2 ANDAND exp3
                    {
                        $$ = new Node(_EXP_, "&&");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp3
                    {
                        $$ = $1;
                    };

exp3                : exp3 OR_OP exp4
                    {
                        $$ = new Node(_EXP_, "|");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp4
                    {
                        $$ = $1;
                    };

exp4                : exp4 AND_OP exp5
                    {
                        $$ = new Node(_EXP_, "&");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp5
                    {
                        $$ = $1;
                    };

exp5                : exp5 EQ exp6
                    {
                        $$ = new Node(_EXP_, "==");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp5 NOTEQ exp6
                    {
                        $$ = new Node(_EXP_, "!=");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6
                    {
                        $$ = $1;
                    };

exp6                : exp6 LT exp7
                    {
                        $$ = new Node(_EXP_, "<");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 GT exp7
                    {
                        $$ = new Node(_EXP_, ">");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 LTEQ exp7
                    {
                        $$ = new Node(_EXP_, "<=");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 GTEQ exp7
                    {
                        $$ = new Node(_EXP_, ">=");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp7
                    {
                        $$ = $1;
                    };

exp7                : exp7 SHL_OP exp8
                    {
                        $$ = new Node(_EXP_, "<<");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp7 SHR_OP exp8
                    {
                        $$ = new Node(_EXP_, ">>");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp8
                    {
                        $$ = $1;
                    };

exp8                : exp8 PLUSOP exp9
                    {
                        $$ = new Node(_EXP_, "+");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp8 MINUSOP exp9
                    {
                        $$ = new Node(_EXP_, "-");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp9
                    {
                        $$ = $1;
                    };

exp9                : exp9 MUL_OP exp10
                    {
                        $$ = new Node(_EXP_, "*");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp9 DIV_OP exp10
                    {
                        $$ = new Node(_EXP_, "/");
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp10
                    {
                        $$ = $1;
                    };

exp10               : NOT_OP exp11
                    {
                        // To be edited
                        $$ = $2;
                    };
                    | MINUSOP exp11 
                    {
                        // To be edited
                        assert($2->type == _INT_NUM_);
                        $2->int_val = -1 * $2->int_val;
                        $$ = $2;
                    };
                    | exp11 
                    {
                        $$ = $1;
                    };

exp11               : ID LSQUARE exp RSQUARE
                    {
                        Node *id_node = new Node(_ID_, *($1));
                        $$ = new Node(_ARRAY_);
                        $$->left = id_node;
                        $$->right = $3;
                    };
                    | INT_NUM
                    {
                        $$ = new Node(_INT_NUM_, $1);
                    };
                    | ID
                    {
                        $$ = new Node(_ID_, *($1));
                    };
                    | LPAREN exp RPAREN
                    {
                        $$ = $2;
                    };
%%

int main(int argc, char *argv[]) {

    /* yydebug = 1; */

    extern FILE *yyin;

    if (argc < 2) {
        yyparse();
        mips.print();

        return 0;

    } else {
        yyin = fopen(argv[1], "r");

        yyparse();

        fclose(yyin);

        mips.print_id_list();
        mips.sym_table->print_table();
        mips.print();

        return 0;
    }
}

void yyerror(char const *s) {
    printf("%s\n", s);
}