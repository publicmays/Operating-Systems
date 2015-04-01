
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
		currentArgs[i] = NULL;
		my_setenv.args[i] = NULL;
		my_printenv.args[i] = NULL;
		my_unsetenv.args[i] = NULL;
		my_cd.args[i] = NULL;
		my_alias.args[i] = NULL;
		my_unalias.args[i] = NULL;
		my_bye.args[i] = NULL;

	/*	builtInTable[0].args[i] = NULL;
		builtInTable[1].args[i] = NULL;
		builtInTable[2].args[i] = NULL;
		builtInTable[3].args[i] = NULL;
		builtInTable[4].args[i] = NULL;
		builtInTable[5].args[i] = NULL;
*/
	}

}