%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#define YYSTYPE char*

extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);

%}


%union	
{
	int my_number;
	char* my_string;
}

%token <my_string> QUOTED
%token <my_string> WORD 
%token <my_number> NUMBER 


%%
commands: /* empty */ 
	| commands command
	;

command:
	echo_function
	;

echo_function: ECHO
			|	ECHO QUOTED

			;


%%























