%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define YYSTYPE char*

extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);
extern int getWordCount();

extern int wordCount;
extern char* firstWord;

%}

%union {
	int my_number;
	char* string;
}

%token <string> WORD
%token <string> QUOTED
%token <string> OPEN_BRACE;
%token <string> CLOSE_BRACE;
%token ECHO NEWLINE

%type <string> word_case 

%%
commands: 
	| commands command 
	;

command:
	word_case | open_brace_case | close_brace_case
	;

word_case: WORD {
			if(wordCount++ == 0 ) {
				firstWord = $1;
			}
			// printf("%d : %s", wordCount, firstWord);
			};

open_brace_case: OPEN_BRACE;
close_brace_case: CLOSE_BRACE;

%%























