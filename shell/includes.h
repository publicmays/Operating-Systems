#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "unistd.h"

#define allocate(t)		(t *)malloc(sizeof(t))
/* getCommand() */
#define FALSE			0
#define TRUE			1
#define OK 				2
#define ERRORS 			3
#define BYE 			4


/* MAX */
#define MAX_PROMPT_LENGTH	500
#define MAX_BUILT_IN_COMMANDS 7
#define MAX_ALIAS 100
#define MAXARGS 300
#define MAXPATH 50

/* Data Structures */
typedef struct {
	char *args[MAXARGS];
} argTable;

typedef struct {
	char *commandName;
	int remote;
	int inputFileDirectory;
	int outputFileDirectory;
	int numArgs;
	argTable *argTablePtr;
} command;

typedef struct {
	int used;		// when user calls alias ++used, unalias --used
	char *aliasName;
	char *aliasContent;
} alias;

/************** Globals **************/
/* Prompt */
static int globalReadOffset;
static char promptResponse[500];

int wordCount = 0;
char* firstWord;

char* path;
char* home;
int cmd;
/* Built in commands */
static command my_setenv;
static command my_printenv;
static command my_unsetenv;
static command my_cd;
static command my_alias;
static command my_unalias;
static command my_bye;

/* Externs in main.c */
static command builtInTable[MAX_BUILT_IN_COMMANDS];
static alias aliasTable[MAX_ALIAS];


/********* Function Prototypes *********/
/* Initialization */
void initializeBuiltInCommands();
void initializeBuiltInTable();
void initializeAliasTable();

/* Prompt */
void printPrompt();
int getCommand();
int getWordCount();
void understand_errors();
void init_scanner_and_parse();
int yyparse();
int readInputForLexer(char *buffer, int *numBytesRead, int maxBytesToRead);


/********* Functions *********/

void shell_init() {
	initializeBuiltInCommands();
	initializeBuiltInTable();
	initializeAliasTable();
	
	/* Initialize all our tables and variables */
	/* Initialize all variables */
	path = allocate(char);
	home = allocate(char);
	strcpy(path, getenv("PATH"));
	strcpy(home, getenv("HOME"));

	/* If user hits ctrl + c, find a way to disable all exits */
	cmd = -1;
	/* 
		0 - successful
		1 - parse failed, invalid input
		2 - failed due to memory exhaustion
	*/
}

int getCommand() {
	init_scanner_and_parse();	// TODO
	if(yyparse() != 0) {
		// unsuccessfull
		understand_errors(); // YYABORT - 1
		return ERRORS;
	}
	else {
		/* check the command table that we build,
		if first command is bye, return bye 
		
		DOUBLE CHECK
		if(commandTable[0]*){
			return BYE;
		}
		*/
		
		return OK;
	}
}

void processCommand() {
	/*if(builtin) {
		do_it();
	}
	else {
		execute_it();
	}*/
}
void understand_errors() {}
void init_scanner_and_parse() {}

/* Initializing all built in commands */
void initializeBuiltInCommands() {
	/* Static - Only can initialize these variables once */
	my_setenv.commandName = "setenv";
	my_printenv.commandName = "printenv";
	my_printenv.commandName = "unsetenv";
	my_cd.commandName = "cd";
	my_alias.commandName = "alias";
	my_unalias.commandName = "unalias";
	my_bye.commandName = "bye";
}

/* Initialize built in table */
void initializeBuiltInTable() {
	builtInTable[0] = my_setenv;
	builtInTable[1] = my_printenv;
	builtInTable[2] = my_unsetenv;
	builtInTable[3] = my_cd;
	builtInTable[4] = my_alias;
	builtInTable[5] = my_unalias;
	builtInTable[6] = my_bye;
}

/* Initialize alias table */
void initializeAliasTable() {
	int i = 0;
	for(i; i < MAX_ALIAS; ++i) {
		aliasTable[i].used = 0;
		aliasTable[i].aliasName = NULL;
		aliasTable[i].aliasContent = NULL;
	}
}

void printPrompt() {
	int i = 0;
	for(i; i < MAX_PROMPT_LENGTH; ++i) {
			promptResponse[i] = '\0';
	}
	globalReadOffset = 0;
	printf("> ");
	fgets(promptResponse, MAX_PROMPT_LENGTH, stdin);
	wordCount = 0;
	yyparse();
	printf("%d : %s", wordCount, firstWord);
}

int isBuiltInCommand() {
	
}
