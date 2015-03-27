%{
#include <stdio.h>
#include "y.tab.h"

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

%}


%%


echo 										return ECHO;

"\""[^\n]*"\"" 								return QUOTED;
[a-zA-Z]+									return WORD;
[0-9]+ {yylval.my_number = atoi(yytext); return NUMBER;}; /*We store the value for yacc to use in yylval. yytext is the input received by lex*/




%%

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

