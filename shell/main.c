/************************
 * Name : Kathleen Lew 
 * File : main.c
 * Version : 1.0
 * Comments : 
*/
#include "includes.h"
 /* Externs */
extern command builtInTable[MAX_BUILT_IN_COMMANDS];
extern char** aliasTable;

/* Function Prototypes */
int yyparse();
int readInputForLexer(char *buffer, int *numBytesRead, int maxBytesToRead);

int main() {
	shell_init();
	/*while(1) {
		printPrompt();
		switch(cmd = getCommand()){
			case BYE:
				exit();
				break;
			case ERRORS:
				recover_from_errors();
				break;
			case OK:
				processCommand();
				break;
		}
	}*/
}
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

