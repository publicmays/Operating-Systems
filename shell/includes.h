#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "unistd.h"

#define allocate(t)		(t *)malloc(sizeof(t))
/* getCommand() */
#define OK 				0
#define ERROR 			1
/* MAX */
#define MAX_BUILT_IN_COMMANDS 7
#define MAXARGS 300
#define MAXALIAS 100
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
	int used;
	char *aliasName;
	char *aliasContent;
} alias;

/* Globals */
char* path;
char* home;
int cmd;

/* Externs in main.c */
static command builtInTable[MAX_BUILT_IN_COMMANDS];
char** aliasTable;

static int globalReadOffset;
//static const char *globalInputText = "3+4";
static char promptResponse[128];

/********* Function Prototypes *********/
int getCommand();
void understand_errors();
void init_scanner_and_parse();
void initializeCommands();

/********* Functions *********/
void initializeCommands() {
	/* Static - Only can initialize these variables once */
	static command setenv;
	setenv.commandName = "setenv";

	static command printenv;
	printenv.commandName = "printenv";

	static 

}
void shell_init() {
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
		return ERROR;
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
