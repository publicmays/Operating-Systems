int aliasFunction() {
	printf("ALIAS");
	char * name = builtInTable[4].args[0];
	char * word = builtInTable[4].args[1];
	checkVariable(word);
	
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
