compiler: scanner.l parser.y
	@bison -dv -Wother parser.y -Wconflicts-rr
	@flex scanner.l

	@g++ lex.yy.c parser.tab.c struct_functions.cpp -o compiler

scanner:
	@flex scanner.l

	@g++ lex.yy.c -o test

parser:
	@bison -dv -Wother parser.y -Wconflicts-rr -Wcounterexamples
	@flex scanner.l

	@g++ lex.yy.c parser.tab.c util/AST.cpp util/SymbTable.cpp util/MIPS.cpp -o parser

clean:
	@rm -f lex.yy.c parser.tab.c parser.tab.h parser.output compiler.exe parser.exe test.exe parser.exe.stackdump

