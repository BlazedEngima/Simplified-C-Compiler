compiler: scanner.l parser.y
	@bison -dv -Wother parser.y -Wconflicts-rr
	@flex scanner.l

	@g++ lex.yy.c parser.tab.c struct_functions.cpp -o compiler

scanner:
	@flex scanner.l

	@g++ lex.yy.c -o test

clean:
	@rm lex.yy.c parser.tab.c parser.tab.h parser.output compiler

