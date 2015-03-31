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
#define ECHO            7;


/* MAX */
#define MAX_PROMPT_LENGTH	500
#define MAX_BUILT_IN_COMMANDS 8
#define MAX_ALIAS 100
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

/******************************* Globals *******************************/
/* Prompt */
static int globalReadOffset;
static char promptResponse[500];

int wordCount = 0;
char* firstWord;
/* all args after 1st word - line : cmd currentArgs[] */
char* currentArgs[MAXARGS];

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
static command my_echo;

static char* whichLocation = NULL;

/* Externs in main.c */
static command builtInTable[MAX_BUILT_IN_COMMANDS];
static alias aliasTable[MAX_ALIAS];
static variable variableTable[MAX_VARIABLES];

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
int setenvFunction();
int unsetenvFunction();
void printenvFunction();
void printaliasFunction();
int aliasFunction();
int unaliasFunction();

int commandArgsLength(int cmd);
void understand_errors();
void init_scanner_and_parse();


void getCurrentDirectory();

char* findWhich();

/********* Functions *********/

void shell_init() {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	initializeBuiltInCommands();
	initializeBuiltInTable();
	initializeAliasTable();
	initializeVariableTable();
	initializeCurrentArgs();
	
	/* Initialize all our tables and variables */
	/* Initialize all variables */
	path = malloc(256*sizeof(char));
	home = allocate(char);
	strcpy(path, getenv("PATH"));
	strcpy(home, getenv("HOME"));

	cmd = -1;

}

int getCommand() {
	// init_scanner_and_parse();	// TODO
    /* YYPARSE 
		0 - successful
		1 - parse failed, invalid input
		2 - failed due to memory exhaustion
	*/
	/* Reset wordCount back to 0 because newline */
	wordCount = 0;
	initializeCurrentArgs();
	/* When you call yyparse
	 * 1. firstWord is set
	 * 2. currentArgs[] = the rest of the tokens in the line
	 */
	if(yyparse() != 0) {
		// unsuccessfull
		// understand_errors(); // YYABORT - 1
		return ERRORS;
	}
	else {
		if(strcmp(firstWord, builtInTable[6].commandName) == 0){
			return BYE;
		}
		else {

			return OK;
		}
	}
}

void processCommand() {
	/* Debug - see currentArgs[] in line
	 	int i = 0;
	 	while(builtInTable[3].args[i] != NULL) {
		printf("%s\n",builtInTable[3].args[i]);
		++i;
	}*/
	int builtin = isBuiltInCommand();
	
	if(builtin != -1) {

		do_it(builtin);
	}
	/*
	else {
		execute_it();
	}*/
}
void understand_errors() {}
void init_scanner_and_parse() {}

/* Initializing all built in commands */
void initializeBuiltInCommands() {
	int k = 0;
	/* Static - Only can initialize these variables once */
	/* commandName */
	my_setenv.commandName = "setenv";
	my_printenv.commandName = "printenv";
	my_unsetenv.commandName = "unsetenv";
	my_cd.commandName = "cd";
	my_alias.commandName = "alias";
	my_unalias.commandName = "unalias";
	my_bye.commandName = "bye";
	my_echo.commandName = "echo";
	
	/* args[] */
	for(k; k < MAXARGS; ++k){
		my_setenv.args[k] = NULL;
		my_printenv.args[k] = NULL;
		my_unsetenv.args[k] = NULL;
		my_cd.args[k] = NULL;
		my_alias.args[k] = NULL;
		my_unalias.args[k] = NULL;
		my_bye.args[k] = NULL;
		my_echo.args[k] = NULL;
	}
	
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
	builtInTable[7] = my_echo;
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

void initializeVariableTable() {
	int i = 0;
	for(i; i < MAX_VARIABLES; ++i) {
		variableTable[i].used = 0;
		variableTable[i].variable = NULL;
		variableTable[i].word = NULL;
	}
}

void initializeCurrentArgs() {
	int i = 0;
	for(i; i < MAXARGS; ++i) {
		currentArgs[i] = NULL;
		/*my_setenv.args[i] = NULL;
		my_printenv.args[i] = NULL;
		my_unsetenv.args[i] = NULL;
		my_cd.args[i] = NULL;
		my_alias.args[i] = NULL;
		my_unalias.args[i] = NULL;
		my_bye.args[i] = NULL;
		my_echo.args[i] = NULL;*/
		builtInTable[0].args[i] = NULL;
		builtInTable[1].args[i] = NULL;
		builtInTable[2].args[i] = NULL;
		builtInTable[3].args[i] = NULL;
		builtInTable[4].args[i] = NULL;
		builtInTable[5].args[i] = NULL;
		builtInTable[7].args[i] = NULL;
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
}

/* Returns index of built in command in builtInTable, -1 !(builtIn command)*/
int isBuiltInCommand() {

	int index = -1, j, i;

	for(j = 0; j < MAX_BUILT_IN_COMMANDS; ++j){
	if(strcmp(firstWord, builtInTable[j].commandName) == 0) {
			// first word is built in command
			/* set command arguments */
			//commandTemp = builtInTable[j];

			/*Extra blank argument produced with -1, changed to -2*/
			for(i = 0; i < wordCount-2; ++i) {
				builtInTable[j].args[i] = currentArgs[i];
				// 	THIS IS THE ONE THAT PRINTS CORECTLY NOT PARSER.Y
				// printf("%d - %s\n", i, currentArgs[i] );
			}
			index = j;
			break;
		}
	}

	return index;
}


void getCurrentDirectory(){
	char *cwd;
          if ((cwd = getcwd(NULL, 64)) == NULL) {
              perror("pwd");
              exit(2);
          }
          (void)printf("%s\n", cwd);
          free(cwd); /* free memory allocated by getcwd() */
}

void do_it(int builtin){
	 // location of which executable
  //  whichLocation = malloc(sizeof(findWhich()));
    //strcpy(whichLocation, findWhich());
	switch(builtin) {
		case 0:
            setenvFunction();
			break;
        case 1:
        	printenvFunction();
            break;
        case 2:
        	unsetenvFunction();
        	// unsetenv();
            break;
        case 3:		
        	cdFunction();
            break;
        case 4:
        	aliasFunction();
            break;
        case 5:
        	unaliasFunction();
            break;
		case 7:
        printf("setenv");
            break;
	}
	char* argv = "ARGUMENT";

  /* if(execl("/bin/echo", "hello", NULL) != -1)
    	printf("success\n\n");
    else
    	printf("2\n\n");
*/
}
/* returns the amount of arguments in line after 1st token */
int commandArgsLength(int cmd) {
	int i = 0;
 	while(builtInTable[cmd].args[i] != NULL) {
		printf("%s\n",builtInTable[cmd].args[i]);
		++i;
	}
	return i;
}
void cdFunction() {
	/* Debug getCurrentDirectory(); */
	int cdIndex = 3;
	int cdArgLength = commandArgsLength(cdIndex);	// #define CD 3


	/* "path", 'path' - 3 tokens */
	if(cdArgLength <= 3){
		// TODO - NEED TO IMPLEMENTE CD ~ !!!!!!!

		/* need to implement a for loop for cd
		*/ 
		//printf("%d\n", cdArgLength);
		//printf("%s\n", builtInTable[cdIndex].args[0]);
		if(cdArgLength == 0){
				// || strcmp(builtInTable[3].args[0], "~")
				printf("hello");
				chdir(home);
				getCurrentDirectory();
		}
		/* cd path argLength = 1*/
		else if(chdir(builtInTable[cdIndex].args[0]) == 0) {
				// sucessfull

				// getCurrentDirectory();
		}
	
		else {
			// go to
			printf("cd - path was not successful.\n");
		}
			// need to check if it's a correct path
	}
	
	else {
		printf("%d\n", cdArgLength);
		printf("%s\n", builtInTable[cdIndex].args[0]);
		printf("more than 1 argument");
	}

}
char* findWhich()
{

	char* concatMe = NULL;
	// char* path = malloc(256*sizeof(char));
	strcpy(path, getenv("PATH"));
	char pathDir[256];

	int j = 0;
	for(j; j < 256; ++j)
	{
		pathDir[j] = '\0';
	}

	strcpy(pathDir, strtok(path, ":"));	

	while(pathDir != NULL)
	{
		if(concatMe)
		{
			free(concatMe);
			concatMe = NULL;
		}

		concatMe = malloc(256*sizeof(char));
		strcpy(concatMe, pathDir);
		strcat(concatMe, "/which");
		printf("\nPathdir now equals %s", concatMe);

		if ( access(concatMe, F_OK) != -1)
		{
			printf("\nWhich found in: %s\n\n", concatMe);
			return concatMe;
		}
		else
		{
			printf("\nWhich not found in: %s\n\n", concatMe);
		}

		int i = 0;
		for(i; i < 256; ++i)
		{
			concatMe[i] = '\0';
		}

		strcpy(pathDir, strtok(NULL, ":"));	
	}

	if(path)
	{
		free(path);
		path = NULL;
	}
}

int setenvFunction() {
	char * variable = builtInTable[0].args[0];
	char * word = builtInTable[0].args[1];
	int cmd = 0;
	int setenv_argLength = commandArgsLength(cmd);
	// printf("variable:%d\n", setenv_argLength);
	//printf("word:%s\n", word);

	if(variable != NULL && word != NULL) {
		int i;

		/*Checks to see if the variable name already exists*/
		for(i = 0; i < MAX_VARIABLES; i++) {
			if(variableTable[i].used == 1) {
				if(strcmp(variableTable[i].variable, variable) == 0) {
					printf("Variable name already exists.\n");
					return -1;
				}
			}
		}
		if(setenv_argLength == 2){
			/*Attempts to put the variable into the table*/
			for(i = 0; i < MAX_VARIABLES; i++) {
				if(variableTable[i].used == 0) {
					variableTable[i].variable = variable;
					variableTable[i].word = word;
					variableTable[i].used = 1;
					return 0;
				}
			}
		}
	}
	printf("Unable to set variable.\n");
	return -1;
}

int unsetenvFunction() {
	char * variable = builtInTable[2].args[0];

	if(variable != NULL) {
		int i;

		/*Attempts to remove variable from the table*/
		for(i = 0; i < MAX_VARIABLES; i++) {
			if(variableTable[i].used == 1) {
				if(strcmp(variableTable[i].variable, variable) == 0) {
					variableTable[i].variable = NULL;
					variableTable[i].word = NULL;
					variableTable[i].used = 0;
					return 0;
				}
			}
		}
	}
	printf("Unable to remove variable.\n");
	return -1;
}

void printenvFunction() {
	int i = 0;

	/*Print variable table*/
	for(i; i < MAX_VARIABLES; i++)
		if(variableTable[i].used == 1)
			printf("%s = %s\n", variableTable[i].variable, variableTable[i].word);
}

int aliasFunction() {
	char * name = builtInTable[4].args[0];
	char * word = builtInTable[4].args[1];

	if(name != NULL && word != NULL) {
		int i;

		/*Checks to see if the alias name already exists*/
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 1) {
				if(strcmp(aliasTable[i].aliasName, name) == 0) {
					printf("Alias name already exists.\n");
					return -1;
				}
			}
		}

		/*Attempts to add the alias to the table*/
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 0) {
				aliasTable[i].aliasName = name;
				aliasTable[i].aliasContent = word;
				aliasTable[i].used = 1;
				return 0;
			}
		}
		printf("Unable to add alias.\n");
		return -1;
	}

	/*P*/ 
	else if(name == NULL && word == NULL) printaliasFunction();
	return -1;
}

int  unaliasFunction() {
	char * name = builtInTable[5].args[0];

	if(name != NULL) {
		int i;
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 1) {
				if(strcmp(aliasTable[i].aliasName, name) == 0) {
					aliasTable[i].aliasName = NULL;
					aliasTable[i].aliasContent = NULL;
					aliasTable[i].used = 0;
					return 0;
				}
			}
		}
		printf("Unable to remove alias.\n");
		return -1;
	}
	return -1;
}

void printaliasFunction() {
	int i = 0;
	for(i; i < MAX_ALIAS; i++)
		if(aliasTable[i].used == 1)
			printf("%s = %s\n", aliasTable[i].aliasName, aliasTable[i].aliasContent);
}