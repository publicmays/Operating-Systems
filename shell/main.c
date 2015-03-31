/************************
 * Name : Kathleen Lew 
 * File : main.c
 * Version : 1.0
 * Comments : include echo in builtin command
 * cd
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

