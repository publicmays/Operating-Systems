#include "define.h"
#include <sys/types.h>
#include <glob.h>
#include <pwd.h>
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
char* tempArgs[MAXARGS];
char tokenWithoutSlash[MAXARGS];
int isCatNull = FALSE;
char * wildCardResults[MAX_WILDCARDS];
int find2Error = FALSE;

/********* Externs - End *********/



/* Pipelining */
pid_t pid[3];

int pipes[MAX_PIPES][2];

/*File IO*/
int canAppend = -1;

/********* Functions *********/

// TO DO
void shell_init() {
	/* START OF SIGNALS */
	// prevent Ctrl + Z, Ctrl + C
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);          
	signal(SIGILL, SIG_IGN);
	signal(SIGTRAP, SIG_IGN);    
	signal(SIGABRT, SIG_IGN);    
	signal(SIGBUS, SIG_IGN);     
	signal(SIGFPE, SIG_IGN);
	signal(SIGKILL, SIG_IGN);    
	signal(SIGUSR1, SIG_IGN);    
	signal(SIGSEGV, SIG_IGN);    
	signal(SIGUSR2, SIG_IGN);
	// signal(SIGPIPE, SIG_IGN);   blocks pipes 
	signal(SIGALRM, SIG_IGN);    
	signal(SIGTERM, SIG_IGN);    
	signal(SIGCHLD, SIG_IGN);
	signal(SIGCONT, SIG_IGN);    
	signal(SIGSTOP, SIG_IGN);       
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);    
	signal(SIGURG, SIG_IGN);     
	signal(SIGXCPU, SIG_IGN);    
	signal(SIGXFSZ, SIG_IGN);
	signal(SIGVTALRM, SIG_IGN);  
	signal(SIGPROF, SIG_IGN);    
	signal(SIGWINCH, SIG_IGN);   
	signal(SIGIO, SIG_IGN);
	signal(SIGPWR, SIG_IGN);     
	signal(SIGSYS, SIG_IGN);     
	signal(SIGRTMIN, SIG_IGN);   
	signal(SIGRTMIN+1, SIG_IGN); 
	signal(SIGRTMIN+2,SIG_IGN);  
	signal(SIGRTMIN+3,SIG_IGN);
  	signal(SIGRTMIN+4,SIG_IGN);
  	signal(SIGRTMIN+5,SIG_IGN);
	signal(SIGRTMIN+6,SIG_IGN);
	signal(SIGRTMIN+7,SIG_IGN);
	signal(SIGRTMIN+8,SIG_IGN);
	signal(SIGRTMIN+9,SIG_IGN);
	signal(SIGRTMIN+10,SIG_IGN); 
	signal(SIGRTMIN+11,SIG_IGN); 
	signal(SIGRTMIN+12,SIG_IGN); 
	signal(SIGRTMIN+13,SIG_IGN);
	signal(SIGRTMIN+14,SIG_IGN); 
	signal(SIGRTMIN+15,SIG_IGN); 
	signal(SIGRTMAX-14,SIG_IGN); 
	signal(SIGRTMAX-13,SIG_IGN);
	signal(SIGRTMAX-12,SIG_IGN); 
	signal(SIGRTMAX-11,SIG_IGN); 
	signal(SIGRTMAX-10,SIG_IGN); 
	signal(SIGRTMAX-9,SIG_IGN);
	signal(SIGRTMAX-8,SIG_IGN);  
	signal(SIGRTMAX-7,SIG_IGN);  
	signal(SIGRTMAX-6,SIG_IGN);  
	signal(SIGRTMAX-5,SIG_IGN);
	signal(SIGRTMAX-4,SIG_IGN);  
	signal(SIGRTMAX-3,SIG_IGN);  
	signal(SIGRTMAX-2,SIG_IGN);  
	signal(SIGRTMAX-1,SIG_IGN);
	signal(SIGRTMAX,SIG_IGN);

	/* END OF CAN'T KILL US NOW */

	initializeEntireLine();
	initializeEntireLine2();
	
	initializeBuiltInCommands();
	initializeBuiltInTable();
	initializeAliasTable();
	initializeVariableTable();
	initializeCurrentArgs();
	initializeCommandTable();
	initializeTempArgs();

	
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
	initializeTempArgs();
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
	
	processTildeExpansion();

	// processEnvironmentVariablesExpansion must go after processTildeExpansion or segfault will happen
	// echo ~ does not work
	processEnvironmentVariablesExpansion();
		
	
	int builtin = isBuiltInCommand();
	
	if(builtin != -1) {

		do_it(builtin);
		
	}
	else {
		if(checkForTrash() == TRUE) {
                        printf("Error executing command.\n");
        }
		else execute_it();
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
			// processAlias();
			return TRUE;
		}
	}
	return FALSE;
}
void processAlias() {
	int aliasIndexFound = -1, i;
	int counter = 0;
		/* If user doesn't input alias as the first command */
		if(strcmp(entireLine[0],my_alias.commandName) != 0 && strcmp(entireLine[0],my_unalias.commandName) != 0) {
			do{
				// printf("%s != ", entireLine[0])
				for(i = 0; i < entireLineLength(); ++i) {
					// find index where alias is found
					aliasIndexFound = isAlias(entireLine[i]);
					// if index is not -1, replace entireLine[i] with aliasContent
					if(aliasIndexFound >= 0){
						entireLine[i] = aliasTable[aliasIndexFound].aliasContent;
					}
				}
				++counter;
			 }while(checkForMoreAliases() == TRUE && counter < MAX_ALIAS);
			if(counter >= MAX_ALIAS) {
				printf("Error - infinite loop of aliases \n");
			}	
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
						return TRUE;
						//processEnvironmentVariablesExpansion();
						//entireLine[i] = variableTable[environIndexFound].word;
					}
 				}

 			}

 			++indexCounter;
 		}
 		
		++i;
	}
	return FALSE;
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

/* Prints Current Directory */
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
	processWildCards();
	processPipes();


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

/* Prints entire Line */
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

/* Prints Entire Line 2 */
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
	int cdArgLength = builtInCommandArgsLength(cdIndex);	// #define  3

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
		int i = 0;
		int j = 0;
		char colon = (char)58;			
		char tilde = (char)126;
		char* homeCopy = home;
		/*Checks to see if the variable name already exists*/
		for(i = 0; i < MAX_VARIABLES; i++) {
			if(variableTable[i].used == 1) {
				if(strcmp(variableTable[i].variable, variable) == 0) {
					printf("Variable name already exists.\n");
					return -1;
				}
			}
		}
		/*Attempts to put the variable into the table*/
		if(setenv_argLength == 2){
			/*int length = strlen(word);
			int totalLength = strlen(word);
			char wordCopy[length];
			strcpy(wordCopy, word);
			int tildeCount = 0;
			// if :~
			for(i = 0; i < length-1; ++i) {
				if(wordCopy[i] == colon) {
					if(wordCopy[i+1] == tilde) {
						++tildeCount;
					}
				}
			}
			totalLength = strlen(word) + tildeCount*strlen(home);
			for(i = 0; i < totalLength; ++i) {
				if(wordCopy[i] == colon) {
					if(wordCopy[i+1] != NULL) {
						if(wordCopy[i+1] == tilde) {
							for(j=0; j < strlen(home); ++j) {
								wordCopy[i+1] = *homeCopy;
								++homeCopy;
							}
						}
					}
					
				}
			}
			printf("Word : %s\n", wordCopy);*/
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

	char * outfile = getOutputFile();
	FILE *out = NULL;

	if(outfile != NULL && canAppend == FALSE) {
		out = fopen(outfile, "w+");
		for(i; i < MAX_VARIABLES; i++){
			if(variableTable[i].used == 1)
				fprintf(out, "%s = %s\n", variableTable[i].variable, variableTable[i].word);
		}
		fclose(out);
	}
	else if(outfile != NULL && canAppend == TRUE) {
		out = fopen(outfile, "a+");
		for(i; i < MAX_VARIABLES; i++){
			if(variableTable[i].used == 1)
				fprintf(out, "%s = %s\n", variableTable[i].variable, variableTable[i].word);
		}
		fclose(out);
	}
	else {
		for(i; i < MAX_VARIABLES; i++)
			if(variableTable[i].used == 1)
				printf("%s = %s\n", variableTable[i].variable, variableTable[i].word);
	}
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
	if(alias_argLength == 0 || strcmp(entireLine[1], ">") == 0){
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

	/* IO REDIRECTION */
	char * outfile = getOutputFile();
	FILE *out = NULL;

	if(outfile != NULL && canAppend == FALSE) {
		out = fopen(outfile, "w+");
		for(i; i < MAX_ALIAS; i++){
			if(aliasTable[i].used == 1){
				//printf("%d - Used %d", i, aliasTable[i].used);
				fprintf(out, "%s = %s\n", aliasTable[i].aliasName, aliasTable[i].aliasContent);
			}
		}
		fclose(out);
	}
	else if(outfile != NULL && canAppend == TRUE) {
		out = fopen(outfile, "a+");
		for(i; i < MAX_ALIAS; i++){
			if(aliasTable[i].used == 1){
				//printf("%d - Used %d", i, aliasTable[i].used);
				fprintf(out, "%s = %s\n", aliasTable[i].aliasName, aliasTable[i].aliasContent);
			}
		}
		fclose(out);
	}
	else {
		for(i; i < MAX_ALIAS; i++){
			if(aliasTable[i].used == 1){
				//printf("%d - Used %d", i, aliasTable[i].used);
				printf("%s = %s\n", aliasTable[i].aliasName, aliasTable[i].aliasContent);
			}
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

/************************** PROCESS PIPES START *******************************/
void processPipes() {
	int i = 0;
	int append = -1;
	int numArgs = 0;
	int numPipes = 0;
	int pipeCounter = 0;
	int commandCount = 0; 
	int currentCommand = 0;
	int pid = 0;
	int runInBackground = FALSE;

	/*int wowzers = 0;
	for(wowzers; wowzers < MAX_COMMANDS; wowzers++) {
		printf("entireLine[%d]: %s\n", wowzers, entireLine[wowzers]);
	}
*/
	/* find numPipes */

	for(i; i < entireLineLength(); ++i) {
		//printf("entireLine[%d]: %s\n", i,entireLine[i]);
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
			//>processErrorsToFile(); // CHANGE find2Error = TRUE if needed
			/* Check for & in background */
			if(i == entireLineLength()-1 && (strcmp(entireLine[i], "&") == 0)) {
					// printf("Found run in background");
					runInBackground = TRUE;
			}
			
			else if(strcmp(entireLine[i], ">") == 0 || strcmp(entireLine[i], ">>") == 0 || strcmp(entireLine[i], "<") == 0 || strcmp(entireLine[i], "2>&1") == 0 || strcmp(entireLine[i], "2>") == 0) {
				//Break
				i = MAX_COMMANDS;
			}
			else {
				// TODO
				/* Should process wild cards here */
				// processWildCards(i);

				commandTable[pipeCounter].args[numArgs] = entireLine[i];
				//printf("Args for pipe %d: %s\n", pipeCounter, commandTable[pipeCounter].args[numArgs]);	
				++numArgs;
			}
		}	
	}
	/*********** end build command table ***********/
	//printCommandTable();


for(currentCommand; currentCommand <= numPipes; currentCommand++) {
	isCatNull = FALSE;
	initializeTempArgs();
	int pipeReceive[2];
	int pipeSend[2];
	tempArgs[0] = commandTable[currentCommand].commandName;
	// if you're not the ending command, you're creating a new pipe
	if(currentCommand != numPipes) {
		// create sending pipe
		pipe(pipeSend);
	}
	tempArgs[0] = commandTable[currentCommand].commandName;
		
	for(i=0; i <= commandTable[currentCommand].numArgs; ++i) {
		if(i == commandTable[currentCommand].numArgs)
		{
			tempArgs[i+1] = NULL;
		}
		else 
			tempArgs[i+1] = commandTable[currentCommand].args[i];
	}
	if(wordCount > 1) {
		pid = fork();
		/*if(pid > 0) {
			printf("HELLO");
			close(pipeReceive[0]);
			close(pipeReceive[1]);
		}*/
		if(pid < 0) {
			printf("Error pid is negative\n");
		}
		else if(pid == 0) {
			
			char * infile = getInputFile();
			char * outfile = getOutputFile();
			char * errorfile = processErrorsToFile();

			FILE *in = NULL;
			int fd_in = STDIN_FILENO;
			FILE *out = NULL;
			int fd_out = STDOUT_FILENO;
			FILE *error = NULL;
			int fd_error = STDERR_FILENO;
		

			if( numPipes == 0 ) {
				/*Input redirection*/
				if(infile != NULL) {
					in = fopen(infile, "r");
					fd_in = fileno(in);
				}
				if(fd_in != STDIN_FILENO) {
					dup2(fd_in, STDIN_FILENO);
				}

				/*Output redirection*/
				if(outfile != NULL && canAppend == FALSE) {
					out = fopen(outfile, "w+");
					fd_out = fileno(out);
				}
				else if(outfile != NULL && canAppend == TRUE) {
					out = fopen(outfile, "a+");
					fd_out = fileno(out);
				}
				if(fd_out != STDOUT_FILENO) {
					dup2(fd_out, STDOUT_FILENO);
				}
				/*Error redirection*/
				if(errorfile != NULL) {
					error = fopen(errorfile, "w+");
					fd_error = fileno(error);
				}
				if(fd_error != STDERR_FILENO) {
					dup2(fd_error, STDERR_FILENO);
				}
				else dup2(fd_in, STDERR_FILENO);
			
				/* End of Error redirection */
			}
			else if(currentCommand == 0) {
					// printf("firstCommand\n");
					if(infile != NULL) {
						in = fopen(infile, "r");
						fd_in = fileno(in);
					}
					if(fd_in != STDIN_FILENO) {
						dup2(fd_in, STDIN_FILENO);
					}
					else dup2(pipeSend[1], STDOUT_FILENO);

					close(pipeSend[0]);
			}
			else if(currentCommand == numPipes) {
					// printf("lastCommand\n");
					if(outfile != NULL && canAppend == FALSE) {
					out = fopen(outfile, "w+");
					fd_out = fileno(out);
					}
					else if(outfile != NULL && canAppend == TRUE) {
						out = fopen(outfile, "a+");
						fd_out = fileno(out);
					}

					if(fd_out != STDOUT_FILENO) {
						dup2(fd_out, STDOUT_FILENO);
					}
					else dup2(pipeReceive[0], STDIN_FILENO);

					close(pipeReceive[1]);
					
			}
			else {
				// printf("middleCommand\n");
				dup2(pipeReceive[0], STDIN_FILENO);
				dup2(pipeSend[1], STDOUT_FILENO);
				// CLOSING IN CHILD
				close(pipeSend[0]);
				close(pipeReceive[1]);
			}

			/* Makes sure that cat and sort cannot have null arguments */
			if(strcmp(commandTable[currentCommand].commandName, "cat") == 0) {
				if(commandTable[currentCommand].numArgs == 0) {
					printf("Error executing command cat.\n");
					isCatNull = TRUE;
				}
			}
			if(strcmp(commandTable[currentCommand].commandName, "sort") == 0) {
				/* wc -l main.c | sort should only be executing wc -l main.c */
				if(commandTable[currentCommand].numArgs == 0) {
					printf("Error - executing command sort even if you have a command before, because sort has null arguments it should not be executed.\n");
					isCatNull = TRUE;
				}
			}
			int status;
			int builtin = isBuiltInCommandPipeline(currentCommand);
			if(builtin != -1) {
				do_it(builtin);
			}
			else if(isCatNull == FALSE) {
				status = execvp(commandTable[currentCommand].commandName, tempArgs);
				wait();
				printf("Error executing command: %d\n", status);	
			}
			
			_exit(EXIT_FAILURE);
			if(status == -1) {
				fflush(0);
			}
		}
	}
	// shift pipes over for next iteration in parent
	pipeReceive[0] = pipeSend[0];
	pipeReceive[1] = pipeSend[1];

} // End of For Loop 

	if(runInBackground == FALSE) {
		waitpid(pid, NULL, 0);
	}
	else {
		//fflush(0);
		runInBackground = FALSE;
	}
}

/***********************  PROCESS PIPES END ******************/
void initializeTempArgs() {
	int i = 0;
	for(i; i < MAXARGS; ++i) {
		tempArgs[i] = NULL;	
	}	
}

void processTildeExpansion() {
	int foundTilde = FALSE;
	int slashFound = FALSE;
	int i = 1, j = 0, k = 0;
	int firstToken = 0;
	int length = 0;
	int slashIndex;
	char* c;
	char* c1;
	char* c2;

	char* temp[1];
	while(entireLine[i+1] != NULL) {
		/* check for individual "~" */
		if(strcmp("~", entireLine[i]) == 0){
			entireLine[i] = home;
			entireLine[i+1] = NULL;
			break;
		}

	 	else {
		
			/* check for ~word */
			c = entireLine[i];
			c1 = entireLine[i];

			length = strlen(entireLine[i]);
			
			slashIndex = length;
			/* Traverse each word in entireLine[i]
			 * if first token is ~, foundTilde = TRUE
			 * if / is found, slashIndex = i
			 */

			for(j=0; j < length; ++j) {
		
				// first Token
				if(j == 0) {
					
					firstToken = (int)*c;
					
					// found tilde 
					if(firstToken == 126){
					
						foundTilde = TRUE;
					}
				}
				else {
					// if you find a slash
					if(*c == 47){
						slashFound = TRUE;
						slashIndex = j;
						break;
					}
				}
				++c;
			}
			
			if(foundTilde == TRUE) {

				/* ~hello/fewa -> ~hello
				 * if there was a slash, copy new word without slash into tokenWithoutSlash
				 * if there was no slash, entireLine[i] gets copied fully into variable */
				strncpy(tokenWithoutSlash, entireLine[i], slashIndex);

				// extract word from tilde ~hello -> hello
				memmove(&tokenWithoutSlash[0], &tokenWithoutSlash[0 + 1], strlen(tokenWithoutSlash) - 0);

				struct passwd* pw;
				pw = getpwnam(tokenWithoutSlash);
	            if((pw == NULL)) {
	            	printf("Error - getpwnam unknown user\n");
	            }
	            else {
	            	entireLine[i] = pw->pw_dir;
	            	// printf("%s", entireLine[i]);
	            }
			}
			
	 }
		++i;
	} // end of while loop
	
}


char * getInputFile() {
	int i = 0;

	char * infile = NULL;

	for(i; i < entireLineLength(); i++)
		if(strcmp(entireLine[i], "<") == 0)
			infile = entireLine[i+1];

	return infile;
}

char * getOutputFile() {
	int i = 0;

	char * outfile = NULL;

	for(i; i < entireLineLength(); i++) {
		if(strcmp(entireLine[i], ">") == 0) {
			outfile = entireLine[i+1];
			canAppend = FALSE;
		}
		else if(strcmp(entireLine[i], ">>") == 0) {
			outfile = entireLine[i+1];
			canAppend = TRUE;
		}
	}

	return outfile;
}

/* Has * or ? for glob */
int hasPattern(char* arg, int length){
	int i = 0;
	for(i; i < length; ++i) {
		if(*arg == 42)
			return TRUE;
		if(*arg == 63)
			return TRUE;
		++arg;
	}
	return FALSE;
}


void processWildCards() {
	int i; 
	int finalIndex = 0;


	/*Get the length of the command*/
	int length = entireLineLength();

	for(i = 0; i < entireLineLength(); i++) {
		entireLine2[i] = entireLine[i];
	}

	/*Process wild cards*/
	for(i = 0; i < length; i++) {
		int wordLength = strlen(entireLine2[i]);

		/*Check to see if the current arg contains a wildcard*/
		if (hasPattern(entireLine2[i], wordLength) == TRUE){
			glob_t globbuf;

			// If the arg has a wildcard, check for matches
			if (glob(entireLine2[i], 0, NULL, &globbuf) == 0) {
				 size_t j;
				 int count = 0;

				 //Store matches in a temp array
				 for (j = 0; j < globbuf.gl_pathc; j++) {
				 	wildCardResults[j] = strdup(globbuf.gl_pathv[j]);
				 	count++;
				 }

				 //Copy the matches back into the original command
				 int z;
				 for(z = 0; z < count; z++) {
				 	entireLine[finalIndex] = wildCardResults[z];
				 	finalIndex++;
				 }
			}
			//Couldn't find a match, just stick it back into the args
			else {
				entireLine[finalIndex] = entireLine2[i];
				finalIndex++;
			}
			globfree(&globbuf);
		}

		/*If there wasn't a wildcard, copy the arg from the temp array back into the original*/
		else {
			entireLine[finalIndex] = entireLine2[i];
			finalIndex++;
		}
	}

	/*Terminate the command will a null arg*/
	//entireLine[finalIndex] = "";
}

/* Finds 2>file
 * extracts file */
char * processErrorsToFile() {
	find2Error = FALSE;
	int i = 0, j = 0, k = 0;
	int firstToken = 0;
	int secondToken = 0;
	int thirdToken = 0;
	int length = 0;
	int match1 = FALSE;
	int match2 = FALSE;
	int match3 = FALSE;
	char* c;
	char* c1 = NULL;


	while(entireLine[i+1] != NULL) {
		/* check for 2>file */
		c = entireLine[i];
		c1 = entireLine[i];
		length = strlen(entireLine[i]);

		/* Traverse each word in entireLine[i]
		 * if 1st token is 2, match1 = TRUE
		 * if 2nd token is >, match2 = TRUE
		 */
		for(j=0; j < length; ++j) {
			if(j == 0) {
				firstToken = (int)*c;
				// found '2' 
				if(firstToken == 50)
					match1 = TRUE;
			}
			else if(j == 1) {
				secondToken = (int)*c;
				// found '>'
				if(secondToken == 62)
					match2 = TRUE;
			}
			else if(j == 2) {
				thirdToken = (int)*c;
				// found '>'
				if(thirdToken != 38)
					match3 = TRUE;

				else match3 = FALSE;
			}
			else if(match1 == TRUE && match2 == TRUE && match3 == TRUE){
				break;
			} 
				
			++c;
		}
		
		if(match1 == TRUE && match2 == TRUE && match3 == TRUE) {
			find2Error = TRUE;

			/* 2>file goes to 'file' */
			memmove(&c1[0], &c1[0 + 2], strlen(entireLine[i]) - 1);
			//printf("%s", c1);
        	//entireLine[i] = c1;
        	//printf("%s\n", entireLine[i]);
        	return c1;
		}
		++i;
	} // end of while loop*/
	return NULL;
}

/* Check for trash */
int checkForTrash() {
	int i = 33;
	int end = 48;
	char* c;
	for(i; i < end; ++i) {
		c = (char*)&i;
		if(strcmp(entireLine[0], c)==0){
			return TRUE;
		}
	}
	i = 58;
	end = 65;
	for(i; i < end; ++i) {
		 c = (char*)&i;
		if(strcmp(entireLine[0], c)==0){
			return TRUE;
		}
	}

	i = 91;
	end = 97;
	for(i; i < end; ++i) {
		c = (char*)&i;
		if(strcmp(entireLine[0], c)==0){
			return TRUE;
		}
	}

	i = 123;
	end = 127;
	for(i; i < end; ++i) {
		c = (char*)&i;
		if(strcmp(entireLine[0], c)==0){
			return TRUE;
		}
	}
	return FALSE;
}

/* Returns index of built in command in builtInTable, 
 * Returns -1 if it's not a built-in command */
int isBuiltInCommandPipeline(int currentCommand) {

	int index = -1, j, i;
	int lineArgLength = entireLineLength()-1;
	if(commandTable[currentCommand].commandName != NULL) {
		//printf("el[0] - %s", entireLine[0]);
		for(j = 0; j < MAX_BUILT_IN_COMMANDS; ++j){
			// if first word is built in command, 0 = successful
			if(strcmp(commandTable[currentCommand].commandName, builtInTable[j].commandName) == 0) {
	
				/* if there's arguments after entireLine[0] 
				 * Don't forget to initializeEntireLine() in getCommand 
				 * Set command arguments 
				 * Note - Extra blank argument produced with -1, */
			
					index = j;
					break;
				
			}
		}
	}
	return index;
}

char *replace_char (char *str, char find, char *replace) {
    char *ret=str;
    char *wk, *s;

    wk = s = strdup(str);

    while (*s != 0) {
        if (*s == find){
            while(*replace)
                *str++ = *replace++;
            ++s;
        } else
            *str++ = *s++;
    }
    *str = '\0';
    free(wk);
    return ret;
}