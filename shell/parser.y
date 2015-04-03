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
extern char* firstWord;
extern char* currentArgs[MAXARGS];
extern char* entireLine[MAXARGS];
extern char* checkEnvironmentTokens;
/********* Externs - End *********/

/********* Functions - Begin *********/

char tempEnvironment[3];
 void storePossibleEnvironmentTokens(char c[], int lastIndex){
	// store 1st, second, last token in word
	tempEnvironment[0] = c[0];
	tempEnvironment[1] = c[1];
	tempEnvironment[2] = c[lastIndex];	
} 

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
			// printf("word - ");
			
			/* if word > 3, possible environment variable ${} 
			 * storePossibleEnvironmentTokens 
			 * store 1st, second, last token in word */
			if( (strlen(yylval.string)) > 3){
				storePossibleEnvironmentTokens($1, strlen(yylval.string)-1);
				checkEnvironmentTokens = tempEnvironment;
		
			}
			
			entireLine[wordCount] = $1;

			// printf("%d - %s\n",wordCount, entireLine[wordCount]);
			if(wordCount++ == 0 ) {
				firstWord = $1;
			}
			if(wordCount > 1) {
				/* wordCount = 2, index for currentArgs[2-2] = currentArgs[0] */

				currentArgs[wordCount-2] = $1;
				
			// this prints incorrectly
			// 	 printf("%d - %s\n",wordCount-2,currentArgs[wordCount-2]);
			
			}
	};

quoted_case: QUOTED {
			yyparse();
			printf("%s\n", yylval.string);
			printf("Inside quoted\n");
	};

open_brace_case: OPEN_BRACE;
close_brace_case: CLOSE_BRACE;




%%
























