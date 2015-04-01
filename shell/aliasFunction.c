int aliasFunction() {
	int cmd = 4;
	int alias_argLength = commandArgsLength(cmd);

	char * name = builtInTable[4].args[0];
	char * word = builtInTable[4].args[1];
	checkVariable(name);
	
	if(name != NULL && word != NULL) {
		int i;

		/*Checks to see if the alias name already exists*/
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 1) {
				if(strcmp(aliasTable[i].aliasName, name) == 0) {
					printf("Alias name already exists.\n");
					return FALSE;
				}
			}
		}
		/*Attempts to add the alias to the table*/
		for(i = 0; i < MAX_ALIAS; i++) {
			if(aliasTable[i].used == 0) {
				aliasTable[i].aliasName = name;
				aliasTable[i].aliasContent = word;
				aliasTable[i].used = 1;
				return TRUE;
			}
		}
		printf("Unable to add alias.\n");
		return FALSE;
	}
	/*P*/ 
	else if(name == NULL && word == NULL) printaliasFunction();
	return FALSE;
}


int aliasFunction() {
	/* To do - 
		1. alias d a, alias -> Alias name already exists 
	*/
	char * name;
	char * word; 
	int cmd = 4;
	int alias_argLength = commandArgsLength(cmd);

	// if user types alias, no arguments exist 
	if(alias_argLength == 0){
		printf("Args length == 0\n");
		printaliasFunction();
		return FALSE;
	}
	else if(alias_argLength == 1){
		printf("Error - Formatting : alias variable name\n");
	}

	else if(alias_argLength == 2){
		name = builtInTable[4].args[0];
		word = builtInTable[4].args[1];

		// TO DO - HALF WORKING
		checkVariable(name);
		
		if(name != NULL && word != NULL) {
				int i;

				/*Checks to see if the alias name already exists*/
				for(i = 0; i < MAX_ALIAS; i++) {
					if(aliasTable[i].used == 1) {
						if(strcmp(aliasTable[i].aliasName, name) == 0) {
							commandArgsLength(cmd);
							printf("Alias name already exists.\n");
							return FALSE;
						}
					}
				}
				/*Attempts to add the alias to the table*/
				for(i = 0; i < MAX_ALIAS; i++) {
					if(aliasTable[i].used == 0) {
						aliasTable[i].aliasName = name;
						aliasTable[i].aliasContent = word;
						aliasTable[i].used = 1;

						// initialize currentArgs[]
						// initializeCurrentArgs();
						// rest name & word
						// name = NULL;
						// word = NULL;

						return TRUE;
					}
				}
				printf("Unable to add alias.\n");
				return FALSE;
		}
	}

}

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