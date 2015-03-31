%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXARGS 300

extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);
extern int getWordCount();

extern int wordCount;
extern char* firstWord;
extern char* currentArgs[MAXARGS];

%}

%union {
	int my_number;
	char* string;
}

%token <string> WORD
%token <string> QUOTED
%token <string> OPEN_BRACE;
%token <string> CLOSE_BRACE;
%token NEWLINE

%type <string> word_case 

%%
commands: 
	| commands command 
	;

command:
	word_case | open_brace_case | close_brace_case
	;

word_case: WORD {
			// printf("word\n");
			if(wordCount++ == 0 ) {
				firstWord = $1;
			}
			if(wordCount > 1) {
				/* wordCount = 2, index for currentArgs[2-2] = currentArgs[0] */

				currentArgs[wordCount-2] = $1;
				// debug - printf("%d - %s\n",wordCount-2,currentArgs[wordCount-2]);
			
			}
	};

open_brace_case: OPEN_BRACE;
close_brace_case: CLOSE_BRACE;

%%























