int i = 0;
for(i; i < numCommands; ++i) {
	int pid;

	pid = fork();

	// Inside Parent
	if(pid > 0) {
		// close all pipes
		// advance current command
	}

	else if(currentCommand == firstCommand) {
		dup2(pipe[1], STDOUT_FILENO);
		close(pipe[0]);
	}

	else if(currentCommand == lastCommand) {
		dup2(pipe[0], STDIN_FILENO);
		close(pipe[1]);
	}

	else {
		dup2(pipe[0], STDIN_FILENO);
		dup2(pipe[1], STDOUT_FILENO);
		close(pipe[0]);
		close(pipe[1]);
	}

	execvp(currentCommand, args);

	if(theres no &) {
		continue;
	}
	else
	{
		wait(pid, NULL, 0);
	}
}
