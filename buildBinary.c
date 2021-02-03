#include "asm.h"
#include <math.h>

/*	Input: An entire statement row
	The main function to create a binary word.
	ignores a symbol, checks if the opcode is valid, if so:
	passes the actual word creation process to makeCommand function.
	Returns: the amound of words needed for the statment.
*/
int buildCommandWord(char* statement) {
	char word[STATEMENT_SIZE];
	int data_size=0;
	int isOpcode;
	getWord(statement, word, 0);
	if (!strcmp(word, ",")){
		setError();
		printf("First operand cannot be a comma\n");
		return 0;
	}
	/* if first word is a symbol:
	We move the beginning of the string to after the symbol, 
	practically ignoring the it. */
	if (checkIfSymbol(statement))
		statement+=strlen(word)+1;

	getWord(statement, word, 0);
	if (!strcmp(word, ",")){
		setError();
		printf("First operand cannot be a comma\n");
		return 0;
	}
	isOpcode = checkIfOpcode(word);
	if (!isOpcode){
		setError();
		printf("this opcode \"%s\" is not valid \n", word);
		return 0;
	}

	data_size = makeCommand(statement);
	return data_size;
}


/* Builds a complete command
Input: A statement, not including labels.
Output: - The size of words needed for storage
		- The words themselves, linked in the instruction table.

The function is seperating the work to three smaller functions.
It checks if the opcode relates to None, a single, or two operands, 
	and sends the statement to the correct function.
The function also verfies that the correct amount of operands is supplied.
*/
int makeCommand(char* statement){

	int i;
	int data_size = 1;
	int hasFirstOprandWord, hasSecondOprandWord, hasMiddleWord;
	/* Holds the operands */
	char opcodeWord[STATEMENT_SIZE];
	char firstOprandWord[STATEMENT_SIZE];
	char secondOprandWord[STATEMENT_SIZE];
	char middleWord[STATEMENT_SIZE]; /* For comma */

	char* first_inst[] = {"mov", "cmp", "add", "sub", "lea"};
	char* second_inst[] = {"clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
	char* third_inst[] = {"rts", "stop"};

	/* Get the 3 words from the statement */
	getWord(statement, opcodeWord, 0);
	hasFirstOprandWord = getWord(statement, firstOprandWord, 1);
	hasMiddleWord = getWord(statement, middleWord, 2); /* Should be comma if two operands are required */
	hasSecondOprandWord = getWord(statement, secondOprandWord, 3);
	if (getWord(statement, secondOprandWord, 4)){
		setError();
		printf("Got too many oprands or extra comma\n");
		return 0;
	}
		
	/* Class 3: NO arguments */
	for (i=0; i<(sizeof(third_inst)/sizeof(*third_inst)); i++){
		if (!strcmp(opcodeWord, third_inst[i])){
			if (hasFirstOprandWord){
				setError();
				printf("Command '%s' does not take any arguments\n", opcodeWord);
				return 0;
			}
			else{
				/* Sends to create a statement with no arguments */
				third_inst_maker(opcodeWord);
				return 1;
			}
		}
	}
	/* Class 2: A single argument */
	for (i=0; i<(sizeof(second_inst)/sizeof(*second_inst)); i++){
		if (!strcmp(opcodeWord, second_inst[i])){
			if (hasMiddleWord){
				setError();
				printf("Command '%s' does not take more then one argument.\nGot: %s as a second argument\n", opcodeWord, middleWord);
				return 0;
			}
			else{
				/* Sends to create a statement with two arguments */
				second_inst_maker(opcodeWord, firstOprandWord);
				return 2;
			}
		}
	}
	/* Class 1: Two arguments */
	for (i=0; i<(sizeof(first_inst)/sizeof(*first_inst)); i++){
		if (!strcmp(opcodeWord, first_inst[i])){
			if (strcmp(middleWord, ",") || (!hasSecondOprandWord))
			{
				setError();
				printf("Did not find a comma between two operands.\n");
				return 0;
			}
			else{
				/* Sends to create a statement with two arguments */
				data_size = first_inst_maker(opcodeWord, firstOprandWord, secondOprandWord);
				return data_size;
			}
		}
	}

	return data_size;
}




/*
	Input: an entire instruction
	Checks which kind of instuction it is, and calls the function to handle it.
*/
void buildInstructionWord(char* statement){
	char word[STATEMENT_SIZE];

	getWord(statement, word, 0);
	/* We move to the next set of words. */
	
	if (checkIfSymbol(statement)){ /* if first word is a symbol */
		statement+=strlen(word)+1; /* then advance pointer to after it */
		getWord(statement, word, 0);
	}
	
	if (!strcmp(word, ".data")){
		statement+=strlen(word)+1;
		makeDataInstruction(statement); 
	}
	if (!strcmp(word, ".string")){
		statement+=strlen(word)+1;
		makeStringInstruction(statement); 
	}
}

/* 
	Input: an entire statement  (without a label)
	Creates a row in the data table for	every digit by calling makeDataRow.
*/
void makeDataInstruction(char* statement){
	int i = 0;
	char word[STATEMENT_SIZE];
	while (getWord(statement, word, i++)){
		if (!strcmp(word,",")){
			if (!(i%2==0)){
				setError();
				printf("Commas must come between the integers\n");
			}
		}
		else
			makeDataRow(word);
	}
}

/*
Input: a digit to add to the data table
Adds a single digit to the data table
*/
void makeDataRow(char* word){
	hashPtr ptr;
	if (!(ptr = (hashPtr)malloc(sizeof(hash)))){
		printf ("Could not allocate space, terminating program");
		exit(0);
	}
	/*Assign the values*/
	if (atoi(word) || word[0] == '0'){ 
		if (atoi(word) > (int)pow(2,WORD_SIZE-1)){ /*Checking the number is whithin limits*/
			setError();
			printf("int cannot be larger then 2^%d (%d)\n",WORD_SIZE-1, (int)pow(2,WORD_SIZE-1));
			ptr -> word = (int)pow(2,WORD_SIZE-1);
		}
		else 
			if (atoi(word) < (int)(-1)*pow(2,WORD_SIZE-1)+1){/*Checking the number is whithin limits*/
				setError();
				printf("int cannot be smaller then (-2^%d)+1 (-%d)\n",WORD_SIZE-1, (int)pow(2,WORD_SIZE-1)+1);
				ptr -> word = -((int)pow(2,WORD_SIZE-1)+1);
			}
			else
				ptr -> word = atoi(word); 	/*Assigning values*/
		ptr -> address = DC++;
		ptr -> oprandName = "";
		ptr -> next = NULL;

		/* Attach the row in the right place */
		if(dat_head == NULL){
			dat_head = ptr;
		}else{
			hashPtr row = dat_head;
			while (row-> next !=NULL){
				row = row -> next;
			}
			row-> next = ptr;
		}
		
	}
	else{
		setError();
		printf("Invalid conversion of the string %s to number.\n\
			No row was created.\n",word);
		free (ptr);
	}
}


/* 
	Input: an entire statement  (without a label)
	Creates a row in the data table for	every character by calling makeStrRow.
*/
void makeStringInstruction(char* line){
	int ch, i=0,j=0, started=0;
	while (line[i]!=0){
		ch = line[i++];
		if (ch==' ' || ch=='\t' || ch=='\n')
			continue; /* skip whitespaces until we reach the string */
		else 
			if (ch=='"'){
				started =1;
				break;
			}
		if (!started){
			setError();
			printf("Invalid string (not starting with \" )\n");
			return;
		}
	}
	while (line[i]!=0){
		ch = line[i++];
		if (ch=='"'){
			/* add 0 to the end of the string */
			j++;
			makeStrRow('\0');
			return;
		}		
		else{
			j++; /*Count number of characters in the string*/
			makeStrRow(ch);
		}
	}
}

/*
Input: a character to add to the data table
Adds a single character to the data table
*/
void makeStrRow(int ch){
	hashPtr ptr;
	if (!(ptr = (hashPtr)malloc(sizeof(hash)))){
		printf ("Could not allocate space, terminating program");
		exit(0);
	}
	ptr -> word = ch;
	ptr -> address = DC++;
	ptr -> oprandName = "";
	ptr -> next = NULL;
		
	if(dat_head == NULL){
		dat_head = ptr;
	}else{
		hashPtr row = dat_head;
		while (row-> next !=NULL){
			row = row -> next;
		}
		row-> next = ptr;
	}	
}


/*Takes the binary word, adds and binds in linked list with current IC */
int addCommandWord(int word, char* oprandName){
	/* Allocate space */
	hashPtr ptr;
	char* temp;
	if (!(ptr = (hashPtr)malloc(sizeof(hash)))){
		printf("could not allocate space\nExiting..");
		exit(0);
	}
	/* Insert values */
	ptr -> word = word;
	ptr -> address = START_IC + IC++;
	
	temp = (char*)calloc(SYMBOL_SIZE, sizeof(char));  
	strcpy(temp, oprandName);
	ptr -> oprandName = temp;

	ptr -> next = NULL;
	/*Insert the command row to instruction table*/
	if(in_head == NULL){
		in_head = ptr;
	}else{
	  hashPtr row = in_head;
		while (row-> next !=NULL){
			row = row -> next;
		}
		row-> next = ptr;
	}
	return 0;
}

/* 	Creates the word that is needed regardless of the opcode (for all three types) */
int makeBaseCommand(char* opWord, char* firstOprandWord, char* secondOprandWord){
	int binWord=0;

	binWord = setOpcode(binWord, opWord);
	binWord |= mask2; /* Always on with first instruction word*/

	/*Setting directives for the first operand */
	if (firstOprandWord){
		if (isFirstOprandWordIsSource(opWord)){
			if (isReg(firstOprandWord))
				binWord |= mask10;
			else if (isRegWithStar(firstOprandWord))
				binWord |= mask9;
			else if (isDirectNumber(firstOprandWord))
				binWord |= mask7;
			else
				binWord |= mask8;
		}else {
			if (isReg(firstOprandWord))
				binWord |= mask6;
			else if (isRegWithStar(firstOprandWord))
				binWord |= mask5;
			else if (isDirectNumber(firstOprandWord))
				binWord |= mask3;
			else
				binWord |= mask4;
		}
	}
	
	/*Setting directives for the second operand */
	if (secondOprandWord){
		if (isReg(secondOprandWord))
			binWord |= mask6;
		else if (isRegWithStar(secondOprandWord))
			binWord |= mask5;
		else if (isDirectNumber(secondOprandWord))
			binWord |= mask3;
		else
			binWord |= mask4;		
	}
	
	checkIfBaseCommandInvalid(opWord, binWord);
	
	return binWord;
}


/*	Creates a binary word for no-operand commands by calling makeBaseCommand
	Adds the created word to the instuction table by calling addCommandWord
*/
void third_inst_maker(char* opWord){
	int binWord=0;
	binWord = makeBaseCommand(opWord, NULL, NULL);
	addCommandWord(binWord, "");
}

/*	Creates a binary word for a single operand commands by calling makeBaseCommand and single_inst_maker
	Adds the created word to the instuction table by calling addCommandWord
*/
void second_inst_maker(char* opWord, char* firstOprandWord){
	int binWord=0;
	binWord = makeBaseCommand(opWord, firstOprandWord, NULL);
	addCommandWord(binWord, "");
	if (isDirectNumber(opWord)){
		setError();
		printf("command not understood, a number cannot be an operand or label");
		return;
	}
	else
		single_inst_maker(firstOprandWord, 1);
}

/*	
	Creates a binary word for a two operand commands by calling makeBaseCommand and single_inst_maker
	Adds the created word to the instuction table by calling addCommandWord
*/
int first_inst_maker(char* opWord, char* firstOprandWord, char* secondOprandWord){
	int binWord=0;
	int binWord2=0;
	binWord = makeBaseCommand(opWord, firstOprandWord, secondOprandWord);
	addCommandWord(binWord, "");
	/*If the target is a register...*/
	if (isReg(secondOprandWord) || isRegWithStar(secondOprandWord)){
		/*And the source is a register...*/
		if (isReg(firstOprandWord) || isRegWithStar(firstOprandWord))
		{
			binWord2 = (secondOprandWord[isRegWithStar(secondOprandWord) ? 2 : 1]-'0') <<3; /* Set target reg */
			binWord2 |= (firstOprandWord[isRegWithStar(firstOprandWord) ? 2 : 1]-'0') << 6; /* Set source reg */
			binWord2 |= mask2; /* Set ARE */
			addCommandWord(binWord2, "");
			return 2; /*Both operands are registers, we finished*/
		}
	}
	/* Handle separately */
	single_inst_maker(firstOprandWord, 0);
	single_inst_maker(secondOprandWord, 1);
	return 3;
}
/*
	Input: operand word + is it a source operand 
	Creates a binary word relavant to the needed word: relavant for the operands only,
	and adds it to the instruction table 
*/
void single_inst_maker(char* opWord, int isSourceOprand){
	int binWord = 0;
	
	if (isDirectNumber(opWord)){
		char* num = opWord+1;
		/* Handling the case where the number cannot be represented in 12 bits (+ or -) */
		if (atoi(num) > (int)pow(2,WORD_SIZE-4)){
			setError();
			printf ("The direct number %d is too large to fit in memory\n", atoi(num));
			binWord = (int)pow(2,WORD_SIZE-4) << 3;
		}
		else
			if (atoi(num) < ((-1)*(int)pow(2,WORD_SIZE-4))+1){
				setError();
				printf ("The direct number %d is too small to fit in memory\n", atoi(num));
				binWord = ((-1)*(int)pow(2,WORD_SIZE-4)+1) << 3;
			}
			else
				binWord = atoi(num) << 3;
		binWord |= mask2;
		addCommandWord(binWord, "");
		return;
	}
	if (isReg(opWord) || isRegWithStar(opWord)){
		binWord = (opWord[isRegWithStar(opWord) ? 2 : 1]-'0')<< (isSourceOprand ? 3 : 6); /* hasFirstOperandWord[1] is the reg number*/
	
		binWord |= mask2;
		addCommandWord(binWord, "");
		return;
	}
	else{ /*Must be a label*/
		binWord = getSymbolBinWord(opWord);
		addCommandWord(binWord, opWord);
		return;
	}
}

