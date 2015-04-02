#include "define.h"
/******************************* Globals *******************************/
/* Prompt */
static int globalReadOffset;
static char promptResponse[500];
int wordCount = 0;
char* firstWord;

/* all args after 1st word - line : cmd currentArgs[] */
char* currentArgs[MAXARGS];
/* User inputs entire line array */
char* entireLine[MAXARGS];
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

static char* whichLocation = NULL;

/* Externs in main.c */
static command commandTable[MAX_COMMANDS];
static command builtInTable[MAX_BUILT_IN_COMMANDS];
static alias aliasTable[MAX_ALIAS];
static variable variableTable[MAX_VARIABLES];

/********* Functions *********/

// TO DO
void shell_init() {
	// prevent Ctrl + Z, Ctrl + C
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	initializeEntireLine();
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
	initializeEntireLine();
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

		/* This is only called to handle an alias for bye */
		processAlias();

		
		if(strcmp(entireLine[0], builtInTable[6].commandName) == 0){
			return BYE;
		}
		else {
		// this is working fine - printLineLength();
			return OK;
		}
	}
}

/* Built in, 
 * nonbuilt in - execute 
 */
void processCommand() {

	/* fixed - had to add if(strcmp(entireLine[0],my_alias.commandName) != 0) */
	processAlias(); 
	
	int builtin = isBuiltInCommand();
	
	// 3 - printf("name - %s\n", builtInTable[4].args[0]);
	if(builtin != -1) {

		do_it(builtin);
		
	}
	else {
		execute_it();
	}

	/* Debug - see currentArgs[] in line
	 	int i = 0;
	 		while(builtInTable[3].args[i] != NULL) {
			printf("%s\n",builtInTable[3].args[i]);
			++i;
		}
		int i = 0;
		for(i; i < entireLineLength(); ++i) {
			
			printf("%d - %s \n", i, entireLine[i]);
		}
	*/
}

void processAlias() {
	int aliasIndexFound = -1, i;
	// if entireLine[0] != my_alias.commandName
	if(strcmp(entireLine[0],my_alias.commandName) != 0) {
		//printf("%s != ", entireLine[0])
		for(i = 0; i < entireLineLength(); ++i) {
			// find index where alias is found
			aliasIndexFound = isAlias(entireLine[i]);
			// if index is not -1, replace entireLine[i] with aliasContent
			if(aliasIndexFound >= 0){
				entireLine[i] = aliasTable[aliasIndexFound].aliasContent;
			}
		}
		//printLineLength();	
	}
}

int isAlias(char* c) {
	int i;
	for(i = 0; i < MAX_ALIAS; i++) {
		if(aliasTable[i].used == 1) {
			if(strcmp(aliasTable[i].aliasName, c) == 0) {
				// return index where alias is found
				return i;
			}
		}
	}
	return -1;
}


void understand_errors() {}
void init_scanner_and_parse() {}

/* Returns index of built in command in builtInTable, 
 * Returns -1 if it's not a built-in command */
int isBuiltInCommand() {

	int index = -1, j, i;
	int lineArgLength = entireLineLength()-1;
	if(entireLine[0] != NULL) {
		//printf("el[0] - %s", entireLine[0]);
		for(j = 0; j < MAX_BUILT_IN_COMMANDS; ++j){
			// if first word is built in command, 0 = successful
			if(strcmp(entireLine[0], builtInTable[j].commandName) == 0) {
	
				/* if there's arguments after entireLine[0] 
				 * Don't forget to initializeEntireLine() in getCommand 
				 * Set command arguments 
				 * Note - Extra blank argument produced with -1, */
				if(lineArgLength >= 0) {
					
					for(i = 0; i < lineArgLength; ++i) {
						//printf("[%d]-%s ->", i+1, entireLine[i+1]);
						builtInTable[j].args[i] = entireLine[i+1];
						//printf("[%d]-%s\n",i, builtInTable[j].args[i]);

						// 	THIS IS THE ONE THAT PRINTS CORECTLY NOT PARSER.Y
						// printf("%d - %s\n", i, currentArgs[i] );
					}
					
					index = j;
					break;
				}
			}
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
/* Handles commands except built-in */
void execute_it(){

	// check command accessability & executability
	/*if(!Executable()) { }
	
	// check io file existance in case of io redirection
	if(check_in_file() == SYSERR){ }
	if(check_out_file() == SYSERR) { }

	// build up the pipeline
	for(c = 0; c < currcmd; ++c) {
		// prep args
		if( ... ) {argv }
		else { }
		switch(pid == fork() ) { // fork process returns twice 
			case 0 : 
			switch(whichCommand(c)) {
				case FIRST:
					if(close(1) == SYSCALLER) {

					}
					if(dup(comtab[c].outfd)!= 1) {...}
					if(close(comtab[c+1].infd) == SYSCALLER) {...}
					in_redir();
					break;
				case LAST:
					if(close(0) == SYSCALLER) {

					}
					if(dup(comtab[c].infd) != 0) { ... }
					out_redir();
					break;
				case THE_ONLY_ONE:
					in_redir();
					out_redir();
					break;
				default:
					if(dup2(comtab[c].outfd, 1) == SYSCALLER) {...}
					if(dup2(comtab[c].infd, 0) == SYSCALLER) {...}
					if(close(comtab[c+1].infd) == SYSCALLER) {...}
					break;
			}
		}
	}
*/
}
/* Handles all built in commands */
void do_it(int builtin){
	 // location of which executable
  //  whichLocation = malloc(sizeof(findWhich()));
    //strcpy(whichLocation, findWhich());
    // 2 - printf("name - %s\n", builtInTable[4].args[0]);
   
   
	switch(builtin) {
		case 0:
            setenvFunction();
			break;
        case 1:
        	printenvFunction();
            break;
        case 2:
        	unsetenvFunction();
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
	}

}
/* Returns the amount of arguments in line after 1st token */
int builtInCommandArgsLength(int cmd) {
	int i = 0;
 	while(builtInTable[cmd].args[i] != NULL) {
		// printf("command args - %s\n",builtInTable[cmd].args[i]);
		++i;
	}
	return i;
}

/* Returns the length of entireLine[] */
// HERE
int entireLineLength() {
	int i = 0;
 	while(entireLine[i+1] != NULL) {
 		//printf("%s\n", entireLine[i]);
		++i;
	}

	return i;
}
int printLineLength() {
	int i = 0;
 	while(entireLine[i+1] != NULL) {
 		printf("%s - ", entireLine[i]);
		++i;
	}
	printf("\n");
	return i;
}
void cdFunction() {
	// Debug getCurrentDirectory();
	int cdIndex = 3;
	int cdArgLength = builtInCommandArgsLength(cdIndex);	// #define CD 3

	/* "path", 'path' - 3 tokens */
	if(cdArgLength <= 3){
		// TO DO - NEED TO IMPLEMENTE CD ~ !!!!!!!

		//printf("%d\n", cdArgLength);
	
		if(cdArgLength == 0){
				// || strcmp(builtInTable[3].args[0], "~")
				// printf("cd ArgLength == 0");
				chdir(home);
				getCurrentDirectory();
		}
		/* cd path argLength = 1*/
		if(cdArgLength == 1) {
			if(chdir(builtInTable[cdIndex].args[0]) == 0) {
					// sucessfull
					// printf("args[0] - %s", builtInTable[cdIndex].args[0]);
					getCurrentDirectory();
			}
			else {
				// printf("args[0] - %s", builtInTable[cdIndex].args[0]);
				printf("Error - cd path was not successful.\n");
			}
		}

	}
	
	else {
		// printf("%d\n", cdArgLength);
		//printf("%s\n", builtInTable[cdIndex].args[0]);
		printf("Error - cd does not have more than 1 argument\n");
	}

}


int setenvFunction() {
	char * variable = builtInTable[0].args[0];
	char * word = builtInTable[0].args[1];
	cmd = 0;
	int setenv_argLength = builtInCommandArgsLength(cmd);
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
	char * name;
	char * word; 
	cmd = 4;
	int flag = FALSE;
	int alias_argLength = builtInCommandArgsLength(cmd);
	
	// 1 - printf("name - %s\n", builtInTable[4].args[0]);
	
	// printf("Start of alias - argLength : %d\n", alias_argLength);
	// if user types alias, no arguments exist 
	if(alias_argLength == 0){
		printaliasFunction();
	}
	else if(alias_argLength == 1){
		printf("Error - Formatting : alias variable name\n");
	}

	else if(alias_argLength == 2){
		// builtInTable .args is not being reset
		name = builtInTable[4].args[0];
		word = builtInTable[4].args[1];
		
		// printf("name - %s\n", builtInTable[4].args[0]);
		
		// TO DO - HALF WORKING
		if(checkVariable(name) == FALSE){
			return FALSE;
		}
		if(name != NULL && word != NULL) {
				int i;
				//printf("%s - %s\n", name, word);
				/*Checks to see if the alias name already exists*/
				for(i = 0; i < MAX_ALIAS; i++) {
					if(aliasTable[i].used == 1) {
						//printf("i - %d is used\n", i);
						//printf("alias[i] = %s | %s\n", aliasTable[i].aliasName, name);
						if(strcmp(aliasTable[i].aliasName, name) == 0) {
							// builtInCommandArgsLength(cmd);
							printf("Alias name already exists.\n");
							flag = FALSE;
							return flag;
						}
					}
				}
				/*Attempts to add the alias to the table*/
				for(i = 0; i < MAX_ALIAS; i++) {
					/* This is where the error is, setting everything 
					 in the alias table to 1 */
					if(aliasTable[i].used == 0) {
						// printf("0 - i - %d\n", i);
						aliasTable[i].aliasName = name;
						aliasTable[i].aliasContent = word;
						aliasTable[i].used = 1;

						// initializeEntireLine();
						// initializeCurrentArgs();
						// rest name & word
						// name = NULL;
						// word = NULL;

						flag = TRUE;
						return flag;
						// break;
					}
				}
		}
	}
	else {
		printf("Error - Too  many arguments for alias.\n");
	}

	// alias_argLength = builtInCommandArgsLength(cmd);
	// printf("End of alias - argLength : %d\n", alias_argLength);
	return flag;
}

int unaliasFunction() {
	char * name = builtInTable[5].args[0];
	if(name != NULL) {
		int i;
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 1) {
				if(strcmp(aliasTable[i].aliasName, name) == 0) {
					aliasTable[i].aliasName = NULL;
					aliasTable[i].aliasContent = NULL;
					aliasTable[i].used = 0;
					return TRUE;
				}
			}
		}
		printf("Unable to remove alias.\n");
		return FALSE;
	}
	return FALSE;
}

void printaliasFunction() {
	int i = 0;
	for(i; i < MAX_ALIAS; i++){
		if(aliasTable[i].used == 1){
			//printf("%d - Used %d", i, aliasTable[i].used);
			printf("%s = %s\n", aliasTable[i].aliasName, aliasTable[i].aliasContent);
		}
	}
}

/* TODO - alias fdsa**(), because scanner doesn't take in **(), it just makes alias fdsa */
int checkVariable(char* c) {
	int i = 0, length = strlen(c);
	int firstToken = 0, flag = FALSE;
	for(i; i < length; ++i) {
		// first Token
		if(i == 0) {
			firstToken = (int)*c;
			 // printf("firstToken %d\n", firstToken);
			if( !((firstToken >= 65 && firstToken <= 90) || (firstToken >= 97 && firstToken <= 122)) ){
				// [A-Za-z] = Ascii table 65 - 90, 97- 122
				printf("Error - 1st token doesn't start with [a-zA-Z]\n");
				return FALSE;
			}
		}	
		// traverse through the rest of the tokens
		else {
			if( !( (*c >= 48 && *c<= 57) || ( *c >= 65 && *c <= 90 ) || (*c >= 97 && *c <= 122) || (*c == 95) ) ){
			// [A-Za-z] = Ascii table 65 - 90, 97- 122, [_] = 95
			printf("%d - Error - variable cannot have special characters\n", (int)*c);
			return FALSE;
			}
		}
		// printf("%d - %c\n", (int)*c, *c);
		++c;
	}
	// printf("variable");
	return TRUE;
}


void initializeEntireLine() {
	int i = 0;
	for(i; i < MAXARGS; ++i) {
		entireLine[i] = NULL;
	}
}

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

	
	/* args[] */
	for(k; k < MAXARGS; ++k){
		my_setenv.args[k] = NULL;
		my_printenv.args[k] = NULL;
		my_unsetenv.args[k] = NULL;
		my_cd.args[k] = NULL;
		my_alias.args[k] = NULL;
		my_unalias.args[k] = NULL;
		my_bye.args[k] = NULL;

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
		//currentArgs[i] = NULL;
		/*

		my_setenv.args[i] = NULL;
		my_printenv.args[i] = NULL;
		my_unsetenv.args[i] = NULL;
		my_cd.args[i] = NULL;
		my_alias.args[i] = NULL;
		my_unalias.args[i] = NULL;
		my_bye.args[i] = NULL;*/

		builtInTable[0].args[i] = NULL;
		builtInTable[1].args[i] = NULL;
		builtInTable[2].args[i] = NULL;
		builtInTable[3].args[i] = NULL;
		builtInTable[4].args[i] = NULL;
		builtInTable[5].args[i] = NULL;

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
