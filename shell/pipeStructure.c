
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



	//for(currentCommand; currentCommand <= numPipes; currentCommand++) {
		initializeTempArgs();
		int pipeReceive[2];
		int pipeSend[2];
		// if you're not the ending command, you're not creating a new pipe
		if(currentCommand != numPipes) {
			// create sending pipe
			pipe(pipeSend);
		}
/*
		tempArgs[0] = commandTable[currentCommand].commandName;
		
		for(i=0; i <= commandTable[currentCommand].numArgs; ++i) {
			if(i == commandTable[currentCommand].numArgs)
			{
				tempArgs[i+1] = NULL;
			}
			else 
				tempArgs[i+1] = commandTable[currentCommand].args[i];
		}*/
		/*i = 0;
		while(tempArgs[i] != NULL) {
			printf("TempArgs : %s ", tempArgs[i]);
			++i;
		}
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
			char* s[1];
			s[0] = "echo";
			char* t[2];
			t[0] = "echo";
			t[1] = "hello"; 
			commandTable[currentCommand].commandName = "echo";
			tempArgs[0] = "echo";
			tempArgs[1] = "hello";
			int errorCode = execvp(commandTable[currentCommand].commandName, tempArgs);
			printf("%d", errorCode);
			exit(0);
			wait(pid, NULL, 0);
		}
		// shoft pipes over for next iteration
		// in parent
		pipeReceive[0] = pipeSend[0];
		pipeReceive[1] = pipeSend[1];
		
	// } 
	//wait(pid, NULL, 0);
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