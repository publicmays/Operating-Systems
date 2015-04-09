#include "define.h"
#include <sys/types.h>
/******************************* Globals *******************************/
/* Prompt */
static int globalReadOffset;
static char promptResponse[500];
int wordCount = 0;

/* all args after 1st word - line : cmd currentArgs[] */


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

/********* Externs - Begin *********/
static command commandTable[MAX_COMMANDS];
static command builtInTable[MAX_BUILT_IN_COMMANDS];
static alias aliasTable[MAX_ALIAS];
static variable variableTable[MAX_VARIABLES];
/* User inputs entire line array */
char* entireLine[MAXARGS];
char* entireLine2[MAXARGS];			
// this holds [$,{,}]
static char environmentVariableSyntax[3];
// this checks word[0], word[1], word[length-1] to compare to ${_}
char possibleEnvironmentTokens[3];
char* environmentExpansionVariables[MAXARGS];



/********* Externs - End *********/



/* Pipelining */
pid_t pid[3];

int pipes[MAX_PIPES][2];


/********* Functions *********/

// TO DO
void shell_init() {
	// prevent Ctrl + Z, Ctrl + C
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	initializeEntireLine();
	initializeEntireLine2();
	
	initializeBuiltInCommands();
	initializeBuiltInTable();
	initializeAliasTable();
	initializeVariableTable();
	initializeCurrentArgs();
	initializeCommandTable();

	
	/* Initialize all our tables and variables */
	/* Initialize all variables */
	path = malloc(256*sizeof(char));
	home = allocate(char);
	strcpy(path, getenv("PATH"));
	strcpy(home, getenv("HOME"));

	/* Environment Variable */
	initializePossibleEnvironmentTokens();
	environmentVariableSyntax[0] = '$';
	environmentVariableSyntax[1] = '{';
	environmentVariableSyntax[2] = '}';
	initializeEnvironmentExpansionVariables();

	cmd = -1;

	/* Initialize Environment Variable Expansion */
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
	initializeEntireLine2();
	initializeEnvironmentExpansionVariables();
	initializeCurrentArgs();
	initializeCommandTable();
	
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
			// printEntireLine();
			// printCheckEnvironmentTokens();
			return OK;
		}
	}
}

void processCommand() {
	
	processAlias();
	processEnvironmentVariablesExpansion();
	processPipes();
	
	int builtin = isBuiltInCommand();
	
	if(builtin != -1) {

		do_it(builtin);
		
	}
	else {
		 execute_it();
	}


}
int checkForMoreAliases() {
	// go through each line in entireLine
	// call process alias if you find another alias
	int aliasIndexFound = -1, i;
	for(i = 0; i < entireLineLength(); ++i) {
		// find index where alias is found
		aliasIndexFound = isAlias(entireLine[i]);
		// if index is not -1, replace entireLine[i] with aliasContent
		if(aliasIndexFound >= 0){
			//entireLine[i] = aliasTable[aliasIndexFound].aliasContent;
			processAlias();
			return TRUE;
		}
	}
	return FALSE;
}
void processAlias() {
	int aliasIndexFound = -1, i;
	do{
		/* If user doesn't input alias as the first command */
		if(strcmp(entireLine[0],my_alias.commandName) != 0) {
			// printf("%s != ", entireLine[0])
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
	}while(checkForMoreAliases() == TRUE);
	
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
/*********************** Start of Environment Variable Expansion ${_} ************************/
void processEnvironmentVariablesExpansion(){
	int i = 0, indexCounter = 0;
	int wordLength = 0, environIndexFound = -1;
	char* index;
	do{
		// for every single world in entireLine[]
	 	while(entireLine[i+1] != NULL) {

	 		initializePossibleEnvironmentTokens();
	 		wordLength = strlen(entireLine[i]);
	 		indexCounter = 0;
	 		char possibleEnvironmentVariable[wordLength-3];
	 		
	 		// for every single character in the word
	 		for(index = entireLine[i]; *index; ++index){
	 			// initialize possibleEnvironmentTokens
	 			
	 			// the word can possibly contain ${_}
	 			if(wordLength > 3) {
	 				
	 				

	 				if(indexCounter == 0 || indexCounter == 1 || indexCounter == wordLength-1){
	 					storePossibleEnvironmentTokens(*index, indexCounter, wordLength);
	 				}
	 				else {
	 					// store word[2] to word[lastIndex-1]
	 					possibleEnvironmentVariable[indexCounter-2] = *index;
	 					
	 				}	
	 			}
	 			// if you hit the last index, possible environment tokens is complete
	 			if(indexCounter == wordLength-1){
	 				// temp = possibleEnvironmentVariable;
	 				// printf("%s", temp);
	 				if(isEnvironmentVariable(possibleEnvironmentTokens) == TRUE) {
	 					
	 					environIndexFound = checkVariableTable(possibleEnvironmentVariable, wordLength-3);
	 					// printf("%d", environIndexFound);
	 					if(environIndexFound >= 0){
							entireLine[i] = variableTable[environIndexFound].word;
						}
	 				}

	 			}

	 			++indexCounter;
	 		}
	 		
			++i;
		}
	}while(checkForMoreEnvironmentExpansions() == TRUE);

	// entireLine[i] = per word
	// *index per character in entireLine[i]
	// indexCounter - length per word
}

int checkForMoreEnvironmentExpansions() {
	int i = 0, indexCounter = 0;
	int wordLength = 0, environIndexFound = -1;
	char* index;
	char* temp = malloc(sizeof(char));
	// for every single world in entireLine[]
 	while(entireLine[i+1] != NULL) {

 		initializePossibleEnvironmentTokens();
 		wordLength = strlen(entireLine[i]);
 		indexCounter = 0;
 		char possibleEnvironmentVariable[wordLength-3];
 		
 		// for every single character in the word
 		for(index = entireLine[i]; *index; ++index){
 			// initialize possibleEnvironmentTokens
 			
 			// the word can possibly contain ${_}
 			if(wordLength > 3) {
 				
 				

 				if(indexCounter == 0 || indexCounter == 1 || indexCounter == wordLength-1){
 					storePossibleEnvironmentTokens(*index, indexCounter, wordLength);
 				}
 				else {
 					// store word[2] to word[lastIndex-1]
 					possibleEnvironmentVariable[indexCounter-2] = *index;
 					
 				}	
 			}
 			// if you hit the last index, possible environment tokens is complete
 			if(indexCounter == wordLength-1){
 				// temp = possibleEnvironmentVariable;
 				// printf("%s", temp);
 				if(isEnvironmentVariable(possibleEnvironmentTokens) == TRUE) {
 					
 					environIndexFound = checkVariableTable(possibleEnvironmentVariable, wordLength-3);
 					// printf("%d", environIndexFound);
 					if(environIndexFound >= 0){
						processEnvironmentVariablesExpansion();
						//entireLine[i] = variableTable[environIndexFound].word;
					}
 				}

 			}

 			++indexCounter;
 		}
 		
		++i;
	}
}
int checkVariableTable(char c[], int length) {
	//printf("%s - %zu", c, strlen(c));
	int i;
	for(i = 0; i < MAX_VARIABLES; i++) {
		if(variableTable[i].used == 1) {
			if(strncmp(variableTable[i].variable, c, length) == 0) {
				// return index where environment variable is found
				// printf("%s\n", variableTable[i].variable);
				return i;
			}
		}
	}
	return -1;
}
int isEnvironmentVariable(char inputTokens[3]) {
	if(inputTokens[0] != environmentVariableSyntax[0])
	{
		return FALSE;
	}
	if(inputTokens[1] != environmentVariableSyntax[1])
	{
		return FALSE;
	}
	if(inputTokens[2] != environmentVariableSyntax[2])
	{
		return FALSE;
	}
 	return TRUE;
}

void storePossibleEnvironmentTokens(char index, int indexCounter, int wordLength) {
	if(indexCounter == 0 || indexCounter == 1){
		possibleEnvironmentTokens[indexCounter] = index;
	}
	else {
		possibleEnvironmentTokens[2] = index;
	}
}
void initializePossibleEnvironmentTokens() {
	memset(possibleEnvironmentTokens, 0, 3);
}
/*********************** End of Environment Variable Expansion ${_} ************************/

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
	int i = 0; 

	while(entireLine[i+1] != NULL) {
		entireLine2[i] = entireLine[i];
		++i;
	}

	pid_t pid[MAX_PIPES];
/*
	int c = 0;
	int comds = 2;
	for(c; c < comds; c++) {
		switch(pid[c] = fork()) {
			case 0:
				switch(c) {
					case 0:
						close(STDOUT_FILENO);
						dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
						close(commandTable[currPipe+1].io[0]);
						in_redir();
					case comds - 1:
						close(STDIN_FILENO);
						dup2(commandTable[currPipe].io[0], STDIN_FILENO);
						out_redir();
					default:
						dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
						dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
						close(commandTable[currPipe+1].io[0]);
				}
		}
	}*/

		/* ONE COMMAND EXECVP WORKS *******************************

	pid[0] = fork();

	if(pid[0] == -1) {
		printf("There was an error.\n");
	}

	else if(pid[0] != 0) {
		//printf("In Parent.\n");
	}

	else {
		//printEntireLine2();
		int returnVal = execvp(entireLine[0], entireLine2);

		if(returnVal == -1) 
			printf("Error executing command.\n");

		exit(0);
	} 
	wait();

	*******************************************************************************/

	

	/*FILE *in = NULL;
	FILE *out = NULL;

	int fd_in = STDIN_FILENO;
	int fd_out = STDOUT_FILENO;

	char *infile = "input.txt";
	char *outfile = "output.txt";

	if(infile != NULL) {
		in = fopen(infile, "r");
		fd_in = fileno(in);
	}

	if(outfile != NULL) {
		out = fopen(outfile, "w+");
		fd_out = fileno(out);
	}

	pid_t processID = fork();
	if(processID == 0) {
		if(fd_in != STDIN_FILENO) {
			dup2(fd_in, STDIN_FILENO);
			dup2(fd_in, STDERR_FILENO);
		}
		if(fd_out != STDOUT_FILENO) {
			dup2(fd_out, STDOUT_FILENO);
		}

		while(entireLine[i+1] != NULL) {
			entireLine2[i] = entireLine[i];
			++i;
		}

		int returnVal = execvp(entireLine[0], entireLine2);

		exit(0);
	}
	wait();*/

	/*char* executableLine[entireLineLength() - 1];

	pid_t pids[MAX_COMMANDS];

	int i = 0;
	for(i; i < entireLineLength() -1; i++) {
		executableLine[i] = entireLine[i+1];
	}
	fork();
	execvp(entireLine[0], executableLine);*/
	/*
	switch(pids[i] = fork()) {
		case -1:
			printf("Failed to fork");
			break;
		case 0:
			if(lastPipe >= 0) {
				//Pipe to read from
				dup2(lastPipe, STDIN_FILE_ID);
				close(lastPip);
			}
			if(currPipe[WRITE_END] >= 0) {
				//Pipe to right to
				dup2(currPipe[WRITE_END], STDOUT_FILE_ID);
				close(currPipe[WRITE_END]);
				close(currPipe[READ_END]);
			}
			executeCommand(command);
			exit(0);
			break;
		default:
			if(currPipe[WRITE_END] >= 0)
				close(currPipe[WRITE_END]);
			if(lastPipe >= 0)
				close(lastPipe);

			lastPipe = currPipe[READ_END];
			currPipe[READ_END] = -1;
			currPipe[WRITE_END] = -1;
			break;
	}*/

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
int entireLineLength() {
	int i = 0;
 	while(entireLine[i+1] != NULL) {
		++i;
	}

	return i;
}


int printEntireLine() {
	printf("printEntireLine\n");
	int i = 0;
	char* index;
 	while(entireLine[i+1] != NULL) {
 		
 		printf("%d - %s | ", i, entireLine[i]);
		++i;
	}
	printf("\n");
	return i;
}

int printEntireLine2() {
	printf("printEntireLine2\n");
	int i = 0;
	char* index;
 	while(entireLine2[i] != NULL) {
 		
 		printf("%d - %s | ", i, entireLine2[i]);
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
void initializeEntireLine2() {
	int i = 0;
	for(i; i < MAXARGS; ++i) {
		entireLine2[i] = NULL;
	}
}

void initializeEnvironmentExpansionVariables(){
	int i = 0;
	for(i; i < MAXARGS; ++i) {
		environmentExpansionVariables[i] = NULL;
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

void initializeCommandTable() {
	int i = 0, j = 0; 
	for(i; i < MAX_COMMANDS; ++i) {
		commandTable[i].commandName = NULL;
		commandTable[i].io[0] = 0;
		commandTable[i].io[1] = 1;
		commandTable[i].numArgs = 0;
		for(j; j < MAXARGS; ++j) {
			commandTable[i].args[j] = NULL;
		}
		
	}
}

void printCommandTable() {
	int i = 0, j = 0;
	while(commandTable[i].commandName != NULL) {
		printf("%s | ", commandTable[i].commandName);
		printf("Num args : %d", commandTable[i].numArgs);
		for(j; j < commandTable[i].numArgs; ++j) {
			printf("%s ", commandTable[i].args[j]);	
		}
		++i;
		j = 0;
		printf("\n");
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


void processPipes() {
	int i = 0;
	int append = -1;
	int numArgs = 0;
	int numPipes = 0;
	int pipeCounter = 0;
	int commandCount = 0;
	
	FILE * in = NULL;
	FILE *out = NULL;

	int fd_in = STDIN_FILENO;
	int fd_out = STDOUT_FILENO;

	char *infile = NULL;
	char *outfile = NULL;
	/* find numPipes */
	for(i; i < entireLineLength(); ++i) {
		if(strcmp(entireLine[i], "|") == 0)
			++numPipes;
	}
	if(numPipes == 0){
		commandTable[0].commandName = entireLine[0];
		commandTable[0].numArgs = entireLineLength() - 1;
	}
	/*********** build command table ***********/
	for(i = 0; i < entireLineLength(); i++)
	{	
		
		if(commandCount == 0) {
			commandTable[pipeCounter].commandName = entireLine[i];
			++commandCount;
		}
		else if(strcmp(entireLine[i], "|") == 0) {
			commandTable[pipeCounter].numArgs = numArgs;
			numArgs = 0;
			commandCount = 0;
			++pipeCounter;
			if(pipeCounter == numPipes) {
				commandTable[pipeCounter].numArgs = entireLineLength() - i - 2;
			}
			//printf("pipeCounter : %d , i : %d\n", pipeCounter, i);
			
		}
		else {
			
			commandTable[pipeCounter].args[numArgs] = entireLine[i];
			//printf("Args for pipe %d: %s\n", pipeCounter, commandTable[pipeCounter].args[numArgs]);	
			++numArgs;
		}	
	
		
	}
	// printCommandTable();

	/*for(i; i < entireLineLength(); i++) {
		if(strcmp(entireLine[i], "<") == 0)
			infile = entireLine[i+1];
		if(strcmp(entireLine[i], ">") == 0) {
			outfile = entireLine[i+1];
			append = 0;
		}
		if(strcmp(entireLine[i], ">>") == 0) {
			outfile = entireLine[i+1];
			append = 1;
		}
	}*/
/************************************* START *****************************************/
	int currentCommand = 0;
	int pid;



	for(currentCommand; currentCommand <= numPipes; currentCommand++) {
		int pipeReceive[2];
		int pipeSend[2];

		// printf("currentCommand: %d. numPipes: %d\n", currentCommand, numPipes);

		// if you're not the ending command, you're not creating a new pipe

		if(currentCommand != numPipes) {

			// create sending pipe
			pipe(pipeSend);
		}

		int length = commandTable[currentCommand].numArgs+2;
		
		char* tempArgs[length];

		tempArgs[0] = commandTable[currentCommand].commandName;
		
		//printf("numArgs : %d", commandTable[currentCommand].numArgs);
		// printf("CMD: %s ARGS: ", commandTable[currentCommand].commandName);

		for(i=0; i <= commandTable[currentCommand].numArgs; ++i) {
			if(i == commandTable[currentCommand].numArgs) {
				tempArgs[i+1] = NULL;
			}
			else tempArgs[i+1] = commandTable[currentCommand].args[i];
			//printf("%s ", commandTable[currentCommand].args[i] );
		}
		printf( "\n" );
		

		/* for(i=0; i < length;++i)
			printf("TempArgs : %s ", tempArgs[i]);
		 printf( "\n");*/

		pid = fork();
		if(pid > 0) {
			close(pipeReceive[0]);
			close(pipeReceive[1]);
		}
		else if(pid < 0) {
			printf("Error pid is negative\n");
		}
		else if(pid == 0) {	// child
			
			//if first command,
				//setup input redirection, if applicable

			//...
			// call open, returns file desciprtor
			// dup2(fd_in, STDIN_FILENO);
			// dup2(STDIN_FILENO, STDERR_FILENO)
			if( numPipes == 0 ) {
				//do nothing
			}
				
			else if(currentCommand == 0) {
				printf("firstCommand\n");
				dup2(pipeSend[1], STDOUT_FILENO);
				close(pipeSend[0]);
			}
			else if(currentCommand == numPipes) {
				printf("lastCommand\n");
				dup2(pipeReceive[0], STDIN_FILENO);
				close(pipeReceive[1]);
			}
			else {
				printf("middleCommand\n");
				dup2(pipeReceive[0], STDIN_FILENO);
				dup2(pipeSend[1], STDOUT_FILENO);
				// CLOSING IN CHILD
				close(pipeSend[0]);
				close(pipeReceive[1]);
			}
			int errorCode = execvp(commandTable[currentCommand].commandName, tempArgs);
			exit(0);
			wait();
		}
		// shoft pipes over for next iteration
		// in parent
		pipeReceive[0] = pipeSend[0];
		pipeReceive[1] = pipeSend[1];
		
	}
/******************************************* end ********************************/
/*	if(infile != NULL) {
		in = fopen(infile, "r");
		fd_in = fileno(in);
	}

	if(outfile != NULL && append == 0) {
		out = fopen(outfile, "w+");
		fd_out = fileno(out);
	}
	else if(outfile != NULL && append == 1) {
		out = fopen(outfile, "a+");
		fd_out = fileno(out);
	}
*/


}
/*
void in_redir() {
	FILE *in = NULL;
	int fd_in = STDIN_FILENO;

	char *infile = getInputFile();

	if(infile != NULL) {
		in = fopen(infile, "r");
		fd_in = fileno(in);
	}

	pid_t processID = fork();

	if(processID == 0) {
		if(fd_in != STDIN_FILENO) {
			dup2(fd_in, STDIN_FILENO);
			dup2(fd_in, STDERR_FILENO);
		}
		if(fd_out != STDOUT_FILENO) {
			dup2(fd_out, STDOUT_FILENO);
		}

		int returnVal = execvp(entireLine[0], entireLine2);
		exit(0);
	}
	wait();
}

char * getInputFile() {

	char *infile = NULL;

	for(i; i < entireLineLength(); i++) {
		if(strcmp(entireLine[i], "<") == 0)
			infile = entireLine[i+1];
	}

	return infile;
}


void out_redir() {

}

char * getOutputFile() {

	char *outfile = NULL;

	for(i; i < entireLineLength(); i++) {
		if(strcmp(entireLine[i], ">") == 0) {
			outfile = entireLine[i+1];
		}
		if(strcmp(entireLine[i], ">>") == 0) {
			outfile = entireLine[i+1];
		}
	}

	return outfile;
}
*/

/******************* currPIpe ***************/
/* int currPipe = 0;
	for(currPipe; currPipe <= numPipes; currPipe++) {
		pipe(commandTable[currPipe].io);
		printf("Piped: %d\n", currPipe);

		if(currPipe == 0 && numPipes == 0) {
			execvp(commandTable[currPipe].commandName, commandTable[currPipe].args);
			exit(0);
		}
		if(currPipe == 0 && numPipes > 0) {
			printf("1");
			int pid = fork();
			if(pid == 0) {
				printf("Its the first pipe!\n");
				int i = 0; 

				while(entireLine[i+1] != NULL) {
					entireLine2[i] = entireLine[i];
					++i;
				}

				//close(STDOUT_FILENO);
				dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
				//close(commandTable[currPipe+1].io[0]);
				execvp(commandTable[currPipe].commandName, commandTable[currPipe].args);
				
				exit(0);
			}
			wait();
			//in_redir();
		}
		if(currPipe == numPipes) {
			printf("2");
			printf("Its the last pipe!\n");
			//close(STDIN_FILENO);
			dup2(commandTable[currPipe-1].io[0], STDIN_FILENO);
			execvp(commandTable[currPipe].commandName, commandTable[currPipe].args);
			exit(0);
			//out_redir();
		}
		wait();
		
		else if(numPipes == 1) {
			//in_redir();
			//out_redir();
		}
		else {
			printf("Middle pipe!\n");
			dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
			dup2(commandTable[currPipe].io[1], STDOUT_FILENO);
			close(commandTable[currPipe+1].io[0]);
		}
	}
	*/
