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
extern char* checkEnvironmentTokens[MAXARGS];

/********* Externs - End *********/

/********* Functions - Begin *********/

char tempEnvironment[3];
char* environmentVariables[MAXARGS];


/*int isEnvironmentVariable(char* c){
	if(strcmp(c, environmentVariableSyntax) == 0){
		return 0;
	}
	return 1;
	//printf("%s\n", c);
	// printf("%s |",environmentVariableSyntax);
}*/
void storeEnvironmentVariable(int wordCount, char environmentVar[], int lastIndex){
	int i = 0;
	char c[lastIndex-2];
	for(i; i < lastIndex; ++i) {
		c[i] = environmentVar[i+2];
	}

	environmentVariables[wordCount] = c;

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
			//printf("word - ");
			
			//initializeEnvironmentVariableSyntax();

			entireLine[wordCount] = $1;
			/* Environment Expansion */
			/* if word > 3, possible environment variable ${} 
			 * storePossibleEnvironmentTokens 
			 * store 1st, second, last token in word */

			/*if( (strlen(yylval.string)) > 3){
				storePossibleEnvironmentTokens($1, strlen(yylval.string)-1);
				checkEnvironmentTokens[wordCount] = tempEnvironment;

				if(isEnvironmentVariable(checkEnvironmentTokens[wordCount]) == 0)
				{
					storeEnvironmentVariable(wordCount, $1,strlen(yylval.string)-1);
					printf("%s\n", environmentVariables[wordCount]);	
				}
			}
			 else {
			 	checkEnvironmentTokens[wordCount] = " ";
			 }
			*/
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
























