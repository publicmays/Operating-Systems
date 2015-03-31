%{
#include <stdio.h>
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
#define YYSTYPE char*s

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

extern int wordCount;

%}


%%


								
\{											return OPEN_BRACE;
\}											return CLOSE_BRACE;
"\""[^\n]*"\"" 								return QUOTED; /* To do */
[\/a-zA-Z]+[a-zA-Z0-9\_\-\']					{yylval.string = strdup(yytext);return WORD;};
\n | ";"									{ wordCount = 0; return END;};
[ \t\r\s]+									/* ignore whitespace */
.											/* everything else = error */




%%

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

/* 
TODO - need to fix quotes - path can have 
Variables [a-zA-Z]+[0-9]* */
