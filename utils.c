#include "asm.h"


int isdigit(int);
int isalpha(int);

char *opcode[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

/*
	Input: a string
	Checks whether or not the string is a valid opcode from the opcode list
	Output: int 1 if is register, 0 otherwise.
*/
int checkIfOpcode(char* word){
	int opcode_size = sizeof(opcode) / sizeof(*opcode);
	int i;
	for(i = 0; i < opcode_size; i++){
		if(!strcmp(word, opcode[i])){
			return 1;
		}
	}
	return 0;
}

/*
	Input: a string
	Checks whether or not the string is opcode that has source oprand
	Output: int 1 if is sourceOprand, 0 otherwise.
*/
int isFirstOprandWordIsSource(char* word){
	char *opcode[] = {"mov", "cmp", "add", "sub", "lea"};
	int opcode_size = sizeof(opcode) / sizeof(*opcode);
	int i;
	for(i = 0; i < opcode_size; i++){
		if(!strcmp(word, opcode[i])){
			return 1;
		}
	}
	return 0;
}

/*
	Input: a string
	Checks whether or not the string is representing a register
	Output: int 1 if is register, 0 otherwise.
*/
int isReg(char* word){
	char firstLetter = word[0];
	char secondLetter = word[1];
	return firstLetter == 'r' && strlen(word) == 2 && (secondLetter >= '0' && secondLetter <= '8') ? 1 : 0;
}

/*
	Input: a string
	Checks whether or not the string is representing a register holding an address
	Output: int 1 if is register, 0 otherwise.
*/
int isRegWithStar(char* word){
	char firstLetter = word[0];
	char secondLetter = word[1];
	char thirdLetter = word[2];
	return firstLetter == '*' && 
				 secondLetter == 'r' &&
				 strlen(word) == 3 && 
				 (thirdLetter >= '0' && thirdLetter <= '8') ? 1 : 0;
}

/*
	Input: A word the check if is considered direct number (i.e: #-515)
	Returns int 1 if is a valid direct number, 0 otherwise
*/
int isDirectNumber(char* word){
	int i=0;
	char* alleged_number;
	char firstLetter = word[0];
	if (firstLetter == '#')
	{
		alleged_number = &word[(word[1] == '+' || word[1] =='-') ? 2 : 1];
		
		for (; alleged_number[i]!=0; i++){
			if (!isdigit(alleged_number[i])){
				setError();
				printf("%c violates only digits constraint after '#'\nIn word:%s\n",alleged_number[i], word);
				return 0;
			}
		return 1;
		}	
	}
	return 0;
}

/*Check if the first word seems like a symbol (varifaction later)*/
int checkIfSymbol(char* statement) {
	char word[STATEMENT_SIZE];
	getWord(statement, word, 0);
	return word[strlen(word)-1]==':'? 1:0;
}

/*	Input: An entire row.
	Returns: 1 if the first word after the symbol (if exist) is ".data" or ".string"
	0 otherewise.
*/
int checkIfData(char* statement) {
	char word[STATEMENT_SIZE];
	if (checkIfSymbol(statement)) /* statement starts with a symbol? -> takes the second word */
		getWord(statement,word,1);
	else
		getWord(statement,word,0);
	return !strcmp(word, ".data") || !strcmp(word, ".string");
}

/*Input: An entire row.
Returns: 1 if the first word after the symbol (if exist) is ".extern", 0 otherewise. */
int checkIfExtern(char* statement) {
	char word[STATEMENT_SIZE];
	if (checkIfSymbol(statement))
		getWord(statement,word,1);
	else
		getWord(statement,word,0);
	return !strcmp(word, ".extern");
}

/*Input: An entire row.
Returns: 1 if the first word after the symbol (if exist) is ".entry", 0 otherewise. */
int checkIfEntry(char* statement) {
	char word[STATEMENT_SIZE];
	if (checkIfSymbol(statement)){
		getWord(statement,word,1);
	}
	else
		getWord(statement,word,0);
	return !strcmp(word, ".entry");
}

/*
	input: the symbol char*.
	returns int if the symbol is valid or not 
*/
int isValidSymbol(char* symbol){
	int i = 0;
	tablePtr row = sym_head;
	/*Check symbol length*/
	if (strlen(symbol) >= SYMBOL_SIZE){
		printf("\nlength of symbol %s is too big\n", symbol);
		return 0;
	}
	/*Check start with digit*/
	if (isdigit(symbol[0])){
		printf("\nSymbol cannot start with a digit!\n");
		return 0;
	}
	/*Check if exists*/
	while (row!=NULL){
		if (!strcmp(row->name,symbol)){
			printf("\nSymbol already exist!\n");
			return 0;		
		}
		row = row -> next;
	}
	if (checkIfOpcode(symbol) || isReg(symbol) || isRegWithStar(symbol) || !strcmp(symbol, "data") || !strcmp(symbol, "string")){
		printf("\nAssigned label cannot be opcode or register type or instraction word!\n");
		return 0;
	}

	for (; i < strlen(symbol); i++){
		if (!isalpha(symbol[i]) && !isdigit(symbol[i])){
			printf("\nAssigned label cannot contain special character!\n");
			return 0;
		}
	}


	return 1;
}

