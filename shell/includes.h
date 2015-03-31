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
#define PRINT_ENV       1;
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
static command my_echo;

static char* whichLocation = NULL;
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
int isBuiltInCommand();
void do_it(int builtin);

char* findWhich();

/********* Functions *********/

void shell_init() {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	initializeBuiltInCommands();
	initializeBuiltInTable();
	initializeAliasTable();
	
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
	int builtin = isBuiltInCommand();
	if(builtin != -1) {
		printf("builtin");
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
	/* Static - Only can initialize these variables once */
	my_setenv.commandName = "setenv";
	my_printenv.commandName = "printenv";
	my_unsetenv.commandName = "unsetenv";
	my_cd.commandName = "cd";
	my_alias.commandName = "alias";
	my_unalias.commandName = "unalias";
	my_bye.commandName = "bye";
	my_echo.commandName = "echo";
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

void printPrompt() {
	int i = 0;
	for(i; i < MAX_PROMPT_LENGTH; ++i) {
			promptResponse[i] = '\0';
	}
	globalReadOffset = 0;
	printf("> ");
	fgets(promptResponse, MAX_PROMPT_LENGTH, stdin);
	
	
	// isBuiltInCommand();

	// printf("%d : %s", wordCount, firstWord);

}

int isBuiltInCommand() {
	int j;
	for(j = 0; j < MAX_BUILT_IN_COMMANDS; ++j){
	if(strcmp(firstWord, builtInTable[j].commandName) == 0) {
			// first word is built in command
			printf("It is a built in command\n");
			return j;
			
		}
	}
	
	return -1;
}

void do_it(int builtin){
	 // location of which executable
  //  whichLocation = malloc(sizeof(findWhich()));
    //strcpy(whichLocation, findWhich());
	switch(builtin) {
		case 0:
            printf("setenv");
			//setenv
			break;
        case 1:
        printf("setenv");
            break;
        case 2:
        printf("setenv");
            break;
        case 3:
        printf("Path : %s\n", path);
        printf("Home path : %s\n", home);
        if(chdir("/home/jeffjtd/Documents/Operating-Systems") == 0)
        {
            printf("CWD : %s\n", get_current_dir_name());
        }
            break;
        case 4:
        printf("setenv");
            break;
        case 5:
        printf("setenv");
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
