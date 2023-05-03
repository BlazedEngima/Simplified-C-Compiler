scanner:
	@flex scanner.l

	@g++ lex.yy.c -o test

parser:
	@bison -dv -Wother parser.y -Wconflicts-rr -Wcounterexamples
	@flex scanner.l

	@g++ lex.yy.c parser.tab.c util/AST.cpp util/SymbTable.cpp util/MIPS.cpp -o compiler

clean:
	@rm -f lex.yy.c parser.tab.c parser.tab.h parser.output compiler.exe parser.exe test.exe parser.exe.stackdump compiler.exe.stackdump
	@rm -f ./testcases/results/*
all: clean parser

