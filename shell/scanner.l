%{
#include <stdio.h>
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
#define YYSTYPE char*s

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)



%}


%%


								
\{											return OPEN_BRACE;
\}											return CLOSE_BRACE;

[<>|\(\)&;] 								{ return yytext[0]; }
\n | ";"									{ wordCount = 0; return END;};
[\"]+[^ \"\n]*\"							{ yylval.string = strdup(yytext); yylval.string[strlen(yylval.string) - 1] = '\0';
											yylval.string[0] = '\0'; yylval.string++; return WORD; } 

[\'][^\'\n]*[\']							{ yylval.string = strdup(yytext); yylval.string[strlen(yylval.string) - 1] = '\0';
											yylval.string[0] = '\0'; yylval.string++; return WORD; }											
[^ t\r\n;]*									{yylval.string = strdup(yytext);return WORD;};

[ \t\r\n]									/* ignore whitespace */
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

