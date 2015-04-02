/************************
 * Name : Kathleen Lew 
 * File : main.c
 * Version : 1.0
 * Comments : 
 * is entireWordLength going to allow | / : > < as entries
 * test isAlias and processAlias
 * just set entireLine = isAlias & processAlias
 * print out entireLine with new alias stuff
 * builtInCommand needs to be reworked
 * need to set everything from entireLine to firstWord & currentArgs
************************/
#include "includes.h"

/************** Externs **************/
/* Prompt */
extern int globalReadOffset;
extern char promptResponse[500];
/* Tables */
extern command builtInTable[MAX_BUILT_IN_COMMANDS];
extern alias aliasTable[MAX_ALIAS];

/* Function Prototypes */
int main() {
	shell_init();
	while(1) {
		printPrompt();
		switch(cmd = getCommand()){
			case BYE:
				exit(0);
				break;
			case ERRORS:
				//recover_from_errors();
				printf("Error - You got us. There was an error.\n");
				break;
			case OK:
				processCommand();
				break;
		}
	}
}

/* Referenced from stack overflow */
int readInputForLexer( char *buffer, int *numBytesRead, int maxBytesToRead ) {
    int numBytesToRead = maxBytesToRead;
    int bytesRemaining = strlen(promptResponse)-globalReadOffset;
    int i;
    if ( numBytesToRead > bytesRemaining ) { numBytesToRead = bytesRemaining; }
    for ( i = 0; i < numBytesToRead; i++ ) {
        buffer[i] = promptResponse[globalReadOffset+i];
    }
    *numBytesRead = numBytesToRead;
    globalReadOffset += numBytesToRead;
    return 0;
}

