%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXARGS 300

/********* Externs - Begin *********/
extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);
extern int getWordCount();

extern int wordCount;

extern char* entireLine[MAXARGS];


/********* Externs - End *********/

/********* Functions - Begin *********/


/********* Functions - End *********/
%}

%union {
	int my_number;
	char* string;
}

%token <string> VARIABLE
%token <string> WORD
%token <string> QUOTED
%token <string> OPEN_BRACE;
%token <string> CLOSE_BRACE;
%token NEWLINE

%type <string> word_case 
%type <string> quoted_case 

%%
commands: 
	| commands command 
	;

command:
	word_case | open_brace_case | close_brace_case | quoted_case
	;

word_case: WORD {
			//printf("word - ");
			
			//initializeEnvironmentVariableSyntax();

			entireLine[wordCount] = $1;
		
			++wordCount;

		
	};

quoted_case: QUOTED {
			yyparse();
			printf("%s\n", yylval.string);
			printf("Inside quoted\n");
	};

open_brace_case: OPEN_BRACE;
close_brace_case: CLOSE_BRACE;




%%
























