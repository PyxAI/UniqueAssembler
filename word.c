#include "asm.h"

/* Global pointers hold the heads of the three tables */
tablePtr sym_head = NULL;
hashPtr dat_head = NULL;
hashPtr in_head = NULL;

/* IC and DC are much more conveniently used globally */
int DC = 0;
int IC = 0;
int hasError = 0;
int currentLine = 0;

/* 	We are using int datatype to represent the 15 bits of the word.
	We will use the 15 rightmost bits for that matter 
	This list of variables will make it easier to turn bits on
*/
int mask0 = 1;
int mask1 = 1 << 1;
int mask2 = 1 << 2;
int mask3 = 1 << 3;
int mask4 = 1 << 4;
int mask5 = 1 << 5;
int mask6 = 1 << 6;
int mask7 = 1 << 7;
int mask8 = 1 << 8;
int mask9 = 1 << 9;
int mask10 = 1 << 10;
int mask11 = 1 << 11;
int mask12 = 1 << 12;
int mask13 = 1 << 13;
int mask14 = 1 << 14;


/* Increase count on the error counter and print the error line*/
void setError(){
	printf("Error in line: %d - ", currentLine);
	hasError++;
}

/* Creates an entry in the symbol table */
void makeSymRow(int isData, int isExternal, char* symbol, int address){
	tablePtr ptr = (tablePtr)malloc(sizeof(table));
	/*Assign the values*/
	char* temp = (char*)calloc(SYMBOL_SIZE, sizeof(char));  
	strcpy(temp, symbol);
	ptr -> name = temp;
	ptr -> address = address;
	ptr -> isData = isData; 
	ptr -> isExternal = isExternal;
	ptr -> isEntry = 0;
	ptr -> next = NULL;
	
	/* Attach the row in the right place */
	if(sym_head == NULL){
		sym_head = ptr;
	}else{
	  tablePtr row = sym_head;
		while (row-> next !=NULL){
			row = row -> next;
		}
		row-> next = ptr;
	}
}

/*Iterates through the statment and registers all symbols found.
If it's external: register all labels found from word 2 onwards
It it isn't: register only the first word in the statement as symbol 
*/
void addSymbol(char* statement, int address, int isData, int isExternal) {
	int i=0;
	char symbol[STATEMENT_SIZE];
	if (isExternal) /*Start from the second word, if it's multiple labels*/
	{
		if (checkIfSymbol(statement)){ /* Even if the symbol is ignored, we must check if it's valid */
			getWord(statement, symbol, 0);
			printf(".extern statements should not start with a label, ignoring..\n");
			if (!isValidSymbol(symbol)){
				setError();
				printf("Not a valid symbol: reason above.\n");
				return;
			}
			i=2;
		}
		else
			i=1;
	}
	while (getWord(statement, symbol, i++)){
		if (!isExternal) /* Removing the ending ':' from the label */
			symbol[strlen(symbol)-1] = 0;
		if (!strcmp(symbol,",")){
			if (!(i%2==1)){
				setError();
				printf("Commas must come between the symbols\n");
				return;
			}
		}
		else
			if (!(isValidSymbol(symbol))){
				setError();
				printf("%s is not a valid symbol\n", symbol);
			}
			else
				makeSymRow(isData, isExternal, symbol, address);

		if (!isExternal) /* Keeps iterating ONLY if it's an external definition for multiple labels */
			return;
	}
}


/*Turns on the bits corresponding to the opcode and returns it */
int setOpcode(int currentWord, char* word){
	
	if(!strcmp(word, "mov"))
		return currentWord;
	
	if(!strcmp(word, "cmp"))
		return currentWord|mask11;
	
	if(!strcmp(word, "add"))
		return currentWord|mask12;
	
	if(!strcmp(word, "sub"))
		return currentWord|mask11|mask12;
	
	if(!strcmp(word, "lea"))
		return currentWord|mask13;
		
	if(!strcmp(word, "clr"))
		return currentWord|mask11|mask13;
			
	if(!strcmp(word, "not"))
		return currentWord|mask12|mask13;
				
	if(!strcmp(word, "inc"))
		return currentWord|mask11|mask12|mask13;
				
	if(!strcmp(word, "dec"))
		return currentWord|mask14;
				
	if(!strcmp(word, "jmp"))
		return currentWord|mask11|mask14;
				
	if(!strcmp(word, "bne"))
		return currentWord|mask12|mask14;
				
	if(!strcmp(word, "red"))
		return currentWord|mask11|mask12|mask14;
					
	if(!strcmp(word, "prn"))
		return currentWord|mask13|mask14;
					
	if(!strcmp(word, "jsr"))
		return currentWord|mask11|mask13|mask14;
					
	if(!strcmp(word, "rts"))
		return currentWord|mask12|mask13|mask14;
					
	if(!strcmp(word, "stop"))
		return currentWord|mask11|mask12|mask13|mask14;
	
	setError();
	printf("opcode Not found!\n");
	return 0;	
}

/* check if the oprand type is invalid for the opcode */
void checkIfBaseCommandInvalid(char* opWord,int binWord){
	int invalid = 0;
	int i;
	char* invalidMask3[] = {"mov", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "jsr"};
	char* invalidMask6[] = { "jmp", "bne", "jsr"};
	char* invalidMask7to10[] = {"clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
	char* invalidMask3to6[] = {"rts", "stop"};

	for (i=0; i<(sizeof(invalidMask3)/sizeof(*invalidMask3)); i++){
		if (!strcmp(opWord, invalidMask3[i])){
			if(binWord&mask3){
				invalid = 1;
			}
		}
	}
	
	for (i=0; i<(sizeof(invalidMask6)/sizeof(*invalidMask6)); i++){
		if (!strcmp(opWord, invalidMask6[i])){
			if(binWord&mask6){
				invalid = 1;
			}
		}
	}
	
	for (i=0; i<(sizeof(invalidMask7to10)/sizeof(*invalidMask7to10)); i++){
		if (!strcmp(opWord, invalidMask7to10[i])){
			if(binWord&mask7 || binWord&mask8 || binWord&mask9 || binWord&mask10){
				invalid = 1;
			}
		}
	}
	
		
	for (i=0; i<(sizeof(invalidMask3to6)/sizeof(*invalidMask3to6)); i++){
		if (!strcmp(opWord, invalidMask3to6[i])){
			if(binWord&mask3 || binWord&mask4 || binWord&mask5 || binWord&mask6){
				invalid = 1;
			}
		}
	}
	
  /* check this speratly instead of doing array with one item*/
	if(!strcmp(opWord, "lea")){
		if(binWord&mask7 || binWord&mask9 || binWord&mask10 ){
			invalid = 1;
		}
	}
	
	if(invalid){
		setError();
		printf("Oprand type is not valid for this opcode: %s \n", opWord);
	}
		
}

/*
	Input: symbol word
	Creates a binary word relavant to the symbol
*/
int getSymbolBinWord(char* symbol){
	int binWord = 0;
	tablePtr row = sym_head;
	
	while (row !=NULL){
		if (!strcmp(symbol, row->name))
		{
			binWord = row -> address << 3;
	
			if (row -> isExternal)
				binWord |= mask0;
			else
				binWord |= mask1;

			return binWord;
		}
		row = row -> next;
	}
	setError();
	printf("Symbol '%s' not found in internal or external lists!\n", symbol);
	return 0;
}

/*	Input: An entire row, and does it contain a symbol
	Output: The memory size in words needed for this statement
*/
int getWordSpace(char* statement, int isSymbol){
	char word[STATEMENT_SIZE], op1[STATEMENT_SIZE], op2[STATEMENT_SIZE], middle[STATEMENT_SIZE];
	int word_num = isSymbol;
	char* first_inst[] = {"mov", "cmp", "add", "sub", "lea"};
	char* second_inst[] = {"clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
	char* third_inst[] = {"rts", "stop"};
	int i;

	/* If it's a symbol (isSymbol=1) then we need the second word (word number 1),
	 if it's not, we need word 0.. */
	if (!getWord(statement, word, word_num))
		return 0;
	for (i=0; i<(sizeof(third_inst)/sizeof(*third_inst)); i++){
		if (!strcmp(word, third_inst[i]))
		{
			return 1;
		}
	}
	/* If we take one argument */
	for (i=0; i<(sizeof(second_inst)/sizeof(*second_inst)); i++){
		if (!strcmp(word, second_inst[i]))
		{
			return 2;
		}
	}
	/* If we take two arguments */
	for (i=0; i<(sizeof(first_inst)/sizeof(*first_inst)); i++){
		if (!strcmp(word, first_inst[i])){
			/* Taking the first argument */
			if (!getWord(statement, op1, word_num+1)){
				printf("Error: Not enough operands for instruction %s\n", word);
				return 0;
			}
			if (getWord(statement, middle, word_num+2))
				if (strcmp(middle,",")){
					return 0;
				}

			/* Taking the second argument */
			if (getWord(statement, op2, word_num+3))
			{
				if ((isReg(op1) || isRegWithStar(op1)) && (isReg(op2) || isRegWithStar(op2)))
						{
							return 2;	
						}				
			}
			else
			{
				setError();
				printf("Not enough operands for instruction %s\n", word);
				return 0;
			}
		return 3;
		}
	}
	return 0;
}


/*input:
line: (statement) the sentence to find a word in
word: pointer to an existing allocated word array to write the word to
desired_word: which word do we want? starting from 0

output: 0 if no such word, 1 if word is found
		The word is written to the previously allocated word variable
*/
int getWord(char* line, char* word, int desired_word){
	int i;
	int len_to_skip;
	char* line_pointer = line;
	for (i=0; i<=desired_word; i++){
		len_to_skip = getOneWord(line_pointer, word);
		line_pointer += len_to_skip;
		if (line + strlen(line) < line_pointer)
			return 0;
	}
	return 1;
}

/*Returns 0 if no word was found
and 1 if a word was written to "word" argument
The function also stops when hits a comma, but if it only encounters a comma, it returns it.
*/
int getOneWord(char* line, char* word)
{
	int ch, started=0, i=0;  /* i is iterating over the line we are searching
	"started" means - did we start to write a new word or no*/
	int j=0; /* j is iterating over the characters we add to the word */

	/*We are first skipping spaces and tabs, and returning
	only a single word at a time
	And also look for commas specifically
	*/
	if (line[i]==0)
		return 1;
	while (line[i]!=0){
		ch = line[i++];
		if (ch==','){ /* If it's a comma, we return only the comma */
			if (!started)
			{
				word[0]=',';
				word[1]='\0';
				return i;
			}
			else{
				word[j]='\0';
				return i-1; /* -1 so we keep the comma for next read */
			}

		}
		if (ch==' ' || ch=='\t' || ch=='\n'){
			if (started){ /*we started reading and hit a space or tab: its a word*/
				word[j]='\0';
				return i;
			}	/*didn't get the first character yet*/
			else{
				continue;
			}
		}
		/* Not a comma or whitespace -> add it to the word */
		word[j++]=ch;
		started = 1;
	}
	if (!started){
		return i;
	}
	word[j]='\0';
	return i;
}


/* add all symbols on the statement operator, could be duplicate symbol */ 
void handleExternStatement(char * statement){

	addSymbol(statement, 0,  0, 1); 					
}

/* 	update the symbols in statement as entry on the symbol table
	Checks if the entry exists as symbol.
*/
void handleEntryStatement(char * statement){
	char symbol[STATEMENT_SIZE];
	int i = 1, found;
	tablePtr row;
	if (checkIfSymbol(statement)){
		getWord(statement, symbol, 0);
		if (!isValidSymbol(symbol))
			setError();
		printf(".entry statements should not start with a label, ignoring..\n");
		i = 2;
	}
	while (getWord(statement, symbol, i++)){
		found = 0;
		row = sym_head;
		while (row != NULL){  
		  if(!strcmp(row->name, symbol)){
				row->isEntry = 1;
				found = 1;
		  }
		  row = row -> next;
		}
		if (!found) /* No match for the entry in the symbol table */
		{
			setError();
			printf("Entry %s not found in symbols provided in current file\n",symbol);
		}
	}
}

/*
Iterates through the symbol and data tables, and updates their address
The updated address will be current + IC (after first run) + Starting IC (100 usually)
*/
void updateDataSymbolTable() {
	tablePtr row = sym_head;
	hashPtr datarow = dat_head;
	
	while (row != NULL){  
    if(row->isData){
    	row->address = row -> address + IC + START_IC;
    }
    row = row -> next;
	}

	while (datarow != NULL){
    	datarow -> address = datarow -> address + IC + START_IC;
    	datarow = datarow -> next;
	}
	
}

/* 	Iterates through the three linked lists, freeing the memory required to create the tables.
	Uses the macro defined in the header */
void freeMem(){
	int count = 0; /* holds how many words were released */
	free_mem_macro(tablePtr, sym_head, count)
	printf("Freed %d words of memory from symbol table\n", count+1);
	free_mem_macro(hashPtr, dat_head, count)
	printf("Freed %d words of memory from data table\n", count+1);
	free_mem_macro(hashPtr, in_head, count)
	printf("Freed %d words of memory from instruction table\n", count+1);
}

