# Simplified C Compiler
CSC4180 | Samuel Theofie - 119010545

This is a compiler written to translate a simplified version of the C language (Tiny C) to the MIPS ISA.

## How to Run
* Run the Makefile with `make all`
* Call the compiler executable with `./compiler <arg>`
* Calling the compiler with an argument will print out the resuling MIPS code into the `./testcases/result` file
* Just calling the compiler without any arguments will let you type in the terminal


## Code Structure
```
class MIPS;
class Node;
class SymTable;
```
The MIPS class is the class that handles code generation into MIPS commands.
The Node class defines the node generated in building the AST. This class will also be the one used as the entry point of the parser.
The SymTable class the class that holds the variable address map for bookkeeping. It is wrapped around a class to add public functions to prevent unsafe accesses to the map.

## Scanner Design
The scanner follows the syntax definitions of the Tiny C language given from assignment 2. The scanner front end is generated using Flex. As expected, variables and integers are expressed in Regex.
```
// Passes text from yytext into yylval.value as an int
yylval.value = atoi(yytext);
yylval.name = new std::string(yytext);
```

## Parser Design
The parser is generated with bison. The grammar rules are augmented to support operator precedence in evaluating expressions. Additionally, we set the precedence for *if-then-else* statements to be right associative to avoid shift-reduce errors in bison using the `%right` and `%prec` directive.
```
%right THEN ELSE
```
This solves the dangling if-else problem by telling bison to prioritize shifting over reducing so that the else statement is bound to the previous if.

## Code Generation
The AST is built bottom up first as expected from a shift-reduce parser. Code generation is called at the root of the Abstract Syntax Tree which is found in the starting grammar rule.
```
prog : var_declarations statements
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
```
The main routine of code generation involves repeatedly placing register values in memory and maintaining their address locations from the symbol table. We use a small set of registers to perform operations while repeatedly calling `lw` , `li`, and `sw`. 

### Optimizations
There are small optimizations in the code generation. For example, the compiler performs integer operations in compile time as to reduce the number of instructions. 
For example
```
a = 3 + 2;
```
is compiled to
```
li $2, 5
```
in the compiler.

### Branching
The main difficulty is in implementing branching instructions. These add additional complexity to the compiler as we need to keep track of possible branches in the MIPS code and where to jump. I referenced gcc MIPS 12.2.0 to get an overall better idea on how to perform branching.
As the statement rule is one of the entry points to the code generation, branching instructions will always be the root of the node, thus the compiler can then easily retrieve the expression condition to branch and the code block.
```
if_statement        : if_stmt %prec THEN
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
```

