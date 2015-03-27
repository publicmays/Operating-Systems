all: shell

shell: scanner.l parser.y main.c includes.h
	yacc -d parser.y
	lex scanner.l
	cc main.c lex.yy.c y.tab.c -o shell
