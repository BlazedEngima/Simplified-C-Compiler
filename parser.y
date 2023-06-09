%code requires{    
    #include <iostream>
    #include <string>
    #include <vector>
    #include <assert.h>
    #include <fstream>
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
    Map declared_id;
    LabelStack exit_labels;
    MIPS mips = MIPS(&sym_table, &declared_id, &exit_labels);
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

%type <node_vector> declaration_list var_declarations var_declaration statements code_block
%type <node> declaration assign_statement write_statement read_statement read_write_statement
%type <node> if_statement if_stmt return_statement control_statement do_while_statement while_statement
%type <node> statement
%type <node> exp exp1 exp2 exp3 exp4 exp5 exp6 exp7 exp8 exp9 exp10 exp11


%right THEN ELSE
%start prog

%%
prog                : var_declarations statements
                    {
                        for (auto it = $1->begin(); it != $1->end(); it++) {
                            (*it)->gen_declare_code(mips);
                        }
                        delete $1;

                        for (auto it = $2->begin(); it != $2->end(); it++) {
                            (*it)->gen_code(mips);
                        }
                        delete $2;
                    };

var_declarations    : var_declaration var_declarations
                    {
                        for (auto it = $1->begin(); it != $1->end(); it++) {
                            $2->push_back(*(it));
                        }
                        $$ = $2;
                        delete $1;
                    };
                    | %empty
                    {
                        $$ = new std::vector<Node*>;
                    }; 

var_declaration     : INT declaration_list SEMI 
                    {
                        $$ = $2;
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
                        $$ = new Node(_ROOT_, _ASSIGN_OP_);
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

code_block          : statement
                    {
                        $$ = new std::vector<Node*>;
                        $$->push_back($1);
                    };
                    | LBRACE statements RBRACE
                    {
                        $$ = $2;
                    };

statements          : statements statement
                    {
                        $1->push_back($2);
                        $$ = $1;
                    };
                    | statement
                    {
                        $$ = new std::vector<Node*>;
                        $$->push_back($1);
                    };

statement           : assign_statement SEMI
                    {
                        $$ = $1;
                    };
                    | control_statement
                    {
                        $$ = $1;
                    };
                    | read_write_statement SEMI
                    {
                        $$ = $1;
                    };
                    | BREAK SEMI {};
                    | SEMI {};

control_statement   : if_statement
                    {
                        $$ = $1;
                    };
                    | while_statement
                    {
                        $$ = $1;
                    };
                    | do_while_statement SEMI
                    {
                        $$ = $1;
                    };
                    | return_statement SEMI
                    {
                        $$ = $1;
                    };

read_write_statement: read_statement
                    {
                        $$ = $1;  
                    };
                    | write_statement
                    {
                        $$ = $1;
                    };

assign_statement    : ID LSQUARE exp RSQUARE ASSIGNOP exp
                    {
                        $$ = new Node(_ROOT_, _ASSIGN_OP_);

                        // Make array node
                        Node *array_node = new Node(_ARRAY_);
                        Node *id_node = new Node(_ID_, *($1));
                        array_node->left = id_node;
                        array_node->right = $3;

                        $$->left = array_node;
                        $$->right = $6; 
                    };
                    | ID ASSIGNOP exp
                    {
                        $$ = new Node(_ROOT_, _ASSIGN_OP_);

                        Node *id_node = new Node(_ID_, *($1));

                        $$->left = id_node;
                        $$->right = $3;
                    };

if_statement        : if_stmt %prec THEN
                    {
                        $1->type = _ROOT_;
                        $$ = $1;
                    };
                    | if_stmt ELSE code_block
                    {
                        $$ = new Node(_ROOT_, _ELSE);
                        $$->left = $1;
                        $$->code_block = $3;
                    };

if_stmt             : IF LPAREN exp RPAREN code_block
                    {
                        $$ = new Node(_EXP_, _IF);
                        $$->left = $3;
                            $$->code_block = $5;
                        };

while_statement     : WHILE LPAREN exp RPAREN code_block
                    {
                        $$ = new Node(_ROOT_, _WHILE);
                        $$->code_block = $5;
                        $$->left = $3;
                    };

do_while_statement  : DO code_block WHILE LPAREN exp RPAREN
                    {
                        $$ = new Node(_ROOT_, _DO);
                        $$->left = $5;
                        $$->code_block = $2;
                    };

return_statement    : RETURN 
                    {
                        $$ = new Node(_ROOT_, _RETURN_);
                    };

read_statement      : READ_ LPAREN ID RPAREN
                    {
                        Node *id_node = new Node(_ID_, *($3));
                        $$ = new Node(_ROOT_, _READ);
                        $$->right = id_node;
                    };

write_statement     : WRITE_ LPAREN exp RPAREN
                    {
                        $$ = new Node(_ROOT_, _WRITE);
                        $$->right = $3;
                    };

exp                 : exp1
                    {
                        $$ = $1;
                    };

exp1                : exp1 OROR exp2
                    {
                        $$ = new Node(_EXP_, _OROR_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp2
                    {
                        $$ = $1;
                    };

exp2                : exp2 ANDAND exp3
                    {
                        $$ = new Node(_EXP_, _ANDAND_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp3
                    {
                        $$ = $1;
                    };

exp3                : exp3 OR_OP exp4
                    {
                        $$ = new Node(_EXP_, _OR_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp4
                    {
                        $$ = $1;
                    };

exp4                : exp4 AND_OP exp5
                    {
                        $$ = new Node(_EXP_, _AND_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp5
                    {
                        $$ = $1;
                    };

exp5                : exp5 EQ exp6
                    {
                        $$ = new Node(_EXP_, _EQ_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp5 NOTEQ exp6
                    {
                        $$ = new Node(_EXP_, _NOTEQ_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6
                    {
                        $$ = $1;
                    };

exp6                : exp6 LT exp7
                    {
                        $$ = new Node(_EXP_, _LT_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 GT exp7
                    {
                        $$ = new Node(_EXP_, _GT_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 LTEQ exp7
                    {
                        $$ = new Node(_EXP_, _LTEQ_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp6 GTEQ exp7
                    {
                        $$ = new Node(_EXP_, _GTEQ_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp7
                    {
                        $$ = $1;
                    };

exp7                : exp7 SHL_OP exp8
                    {
                        $$ = new Node(_EXP_, _SHL_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp7 SHR_OP exp8
                    {
                        $$ = new Node(_EXP_, _SHR_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp8
                    {
                        $$ = $1;
                    };

exp8                : exp8 PLUSOP exp9
                    {
                        $$ = new Node(_EXP_, _PLUSOP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp8 MINUSOP exp9
                    {
                        $$ = new Node(_EXP_, _MINUSOP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp9
                    {
                        $$ = $1;
                    };

exp9                : exp9 MUL_OP exp10
                    {
                        $$ = new Node(_EXP_, _MUL_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp9 DIV_OP exp10
                    {
                        $$ = new Node(_EXP_, _DIV_OP_);
                        $$->left = $1;
                        $$->right = $3;
                    };
                    | exp10
                    {
                        $$ = $1;
                    };

exp10               : NOT_OP exp11
                    {
                        $$ = new Node(_EXP_, _NOT_OP_);
                        $$->right = $2;
                    };
                    | MINUSOP exp11 
                    {
                        $$ = new Node(_EXP_, _MINUSOP_);
                        $$->right = $2;
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
    extern FILE *yyin;

    if (argc < 2) {
        yyparse();
        mips.add_exit();
        mips.print(std::cout);
        return 0;

    } else {
        std::string file_path = std::string(argv[1]);
        int index = file_path.find_last_of("/\\");
        std::string input_file = file_path.substr(index + 1);

        std::string output_file = "./testcases/results/" + input_file + "_output.asm";
        yyin = fopen(argv[1], "r");
        std::ofstream output(output_file);
        yyparse();
        mips.add_exit();
        fclose(yyin);
        mips.print(output);
        output.close();
        return 0;
    }
}

void yyerror(char const *s) {
    printf("%s\n", s);
}