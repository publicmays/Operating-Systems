#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "y.tab.h"
#include "unistd.h"

#define allocate(t)		(t *)malloc(sizeof(t))
/* getCommand() */
#define FALSE			100
#define TRUE			101
#define OK 				102
#define ERRORS 			103
#define BYE 			104

/* Built in Commands */
#define SETENV          0;
#define PRINTENV       	1;
#define UNSETENV        2;
#define CD              3;
#define ALIAS           4;
#define UNALIAS         5;
// BYE


/* MAX */
#define MAX_PROMPT_LENGTH	500
#define MAX_BUILT_IN_COMMANDS 7
#define MAX_ALIAS 5	// 100
#define MAX_VARIABLES 100
#define MAXARGS 300
#define MAXPATH 50

/* Data Structures */
typedef struct {
	char *commandName;
	int remote;
	int inputFileDirectory;
	int outputFileDirectory;
	int numArgs;
	char *args[MAXARGS];
} command;

typedef struct {
	int used;		// when user calls alias ++used, unalias --used
	char *aliasName;
	char *aliasContent;
} alias;

typedef struct {
	int used; 
	char *variable;
	char *word;
} variable;

/******************************* Function Prototypes *******************************/
/* Initialization */
void initializeBuiltInCommands();
void initializeBuiltInTable();
void initializeAliasTable();
void initializeCurrentArgs();
void initializeVariableTable();

/* yyparse*/
int yyparse();
int readInputForLexer(char *buffer, int *numBytesRead, int maxBytesToRead);

/* main.c - Prompt */
void printPrompt();
int getCommand();
void processCommand();

/* in processCommand() */
int isBuiltInCommand();
void do_it(int builtin);

/* do_it(int) */
void cdFunction();
void getCurrentDirectory();
int setenvFunction();
int unsetenvFunction();
void printenvFunction();
void printaliasFunction();
int aliasFunction();
int unaliasFunction();

int checkVariable(char* c);
int commandArgsLength(int cmd);
void understand_errors();
void init_scanner_and_parse();

char* findWhich();