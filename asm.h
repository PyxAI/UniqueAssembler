#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 15 /* Bits allowed to store in a word */
#define STATEMENT_SIZE 81 /* Total max size of statement */
#define SYMBOL_SIZE 32 /* Symbol cannot contain more characters */
#define START_IC 100 /* Can be changed when getting starting address from OS */
#define FILE_NAME_LEN 100 /* Limit to the file name length */
#define MEMORY_SIZE 4096 /* Memory size in words */

/* 	Iterates a linked list of type *type* that starts with the node *head*
	Writes to count_freed the amount of nodes freed
*/
#define free_mem_macro(type, head, count_freed)\
	{\
	type next_node;\
	count_freed = 0 ;\
	if (head)\
		{\
		if (head -> next)\
			{\
			next_node = head -> next;\
			while (next_node){\
				free(head);\
				count_freed++;\
				head = next_node;\
				next_node = next_node -> next;\
			}\
			free(head);\
			}\
		else\
			free(head);\
	}\
}

/* Pointer to table row */
typedef struct tableItem* tablePtr;

/* Datatype for symbol items */
typedef struct tableItem {
	char * name;
	int address;
	int isData;
	int isEntry;
	int isExternal;
	tablePtr next;
} table;

/* Pointer to table row */
typedef struct hashItem* hashPtr;

/* Datatype for data and instruction items */
typedef struct hashItem {
	int address;
	int word;
	char* oprandName; /* For external symbol address */
	hashPtr next;
} hash;

/* The global variables */
extern int DC;
extern int IC;
extern int hasError;
extern int currentLine;
extern tablePtr sym_head;
extern hashPtr dat_head;
extern hashPtr in_head;



/* 	We are using int datatype to represent the 15 bits of the word.
	We will use the 15 rightmost bits for that matter 
	This list of variables will make it easier to turn bits on
*/
extern int mask0;
extern int mask1;
extern int mask2;
extern int mask3;
extern int mask4;
extern int mask5;
extern int mask6;
extern int mask7;
extern int mask8;
extern int mask9;
extern int mask10;
extern int mask11;
extern int mask12;
extern int mask13;
extern int mask14;


/* add all symbols on the statement operator, could be duplicate symbol */ 
void handleExternStatement(char *);


/* 
	check if the oprand type is invalid for the opcode
*/
void checkIfBaseCommandInvalid(char*, int );


/*
	Input: an entire instruction
	Checks which kind of instuction it is, and calls the function to handle it.
*/
void buildInstructionWord(char*);


/*
	Input: symbol word
	Creates a binary word relavant to the symbol
*/
int getSymbolBinWord(char*);


/*	Turn on the error flag and print the error line*/
void setError();


/*Turns on the bits corresponding to the opcode and returns it */
int setOpcode(int, char*);


/*	Iterates through the statment and registers all symbols found.
	If it's external: register all labels found from word 2 onwards
	It it isn't: register only the first word in the statement as symbol 
*/
void addSymbol(char*, int, int, int);


/*
	Iterates through the symbol and data tables, and updates their address
	The updated address will be current + IC (after first run) + Starting IC (100 usually)
*/
void updateDataSymbolTable();


/*	Input: An entire row.
	Returns: 1 if the first word is ".entry", 0 otherewise. */
int checkIfEntry(char*);


/*	Input: An entire row.
	Returns: 1 if the first word is ".extern", 0 otherewise. */
int checkIfExtern(char*);


/*	Input: An entire row.
	Returns: 1 if the first word is ".data", 0 otherewise. */
int checkIfData(char*);


/*
	input: the symbol char*.
	returns int if the symbol is valid or not 
*/
int isValidSymbol(char*);



/* 	Builds a complete command
	Input: A statement, not including labels.
	Output: - The size of words needed for storage
			- The words themselves, linked in the instruction table.
*/
int makeCommand(char*);


/*	Input: An entire statement row
	The main function to create a binary word.
	ignores a symbol, checks if the opcode is valid, if so:
	passes the actual word creation process to makeCommand function.
	Returns: the amound of words needed for the statment.
*/
int buildCommandWord(char*);


/* 
	update the symbols in statement as entry on the symbol table
	*/
void handleEntryStatement(char *);


/*
	Input: A word the check if is considered direct number (i.e: #-515)
	Returns int 1 if is a valid direct number, 0 otherwise
*/
int isDirectNumber(char*);


/*
	Input: a string
	Checks whether or not the string is representing a register
	Output: int 1 if is register, 0 otherwise.
*/
int isReg(char* word);


/*
	Input: a string
	Checks whether or not the string is representing a register holding an address
	Output: int 1 if is register, 0 otherwise.
*/
int isRegWithStar(char*);


/*
	Input: a string
	Checks whether or not the string is opcode that has source oprand
	Output: int 1 if is sourceOprand, 0 otherwise.
*/
int isFirstOprandWordIsSource(char*);


/*
	Input: a string
	Checks whether or not the string is representing a symbol\label (without validation)
	Output: int 1 if is register, 0 otherwise.
*/
int checkIfSymbol(char*);


/*
	Input: a string
	Checks whether or not the string is a valid opcode from the opcode list
	Output: int 1 if is register, 0 otherwise.
*/
int checkIfOpcode(char*);


/*
	Calls the three functions for building our output files independently 
*/
void buildFiles(char*);


/*
	Input: A statement string, an allocated array to hold the output word, 
			int- which word from statment to take)
	Word is defined as separated by whitespaces or comma.
	Returns 0 If there is no word to be found, 1 otherwise + the word itself in the second passed argument
*/
int getWord (char*, char*, int);


/*Returns 0 if no word was found
and 1 if a word was written to "word" argument
The function also stops when hits a comma, but if it only encounters a comma, it returns it.
*/
int getOneWord(char*, char*);


/*
	Input: A statement string, int representing if a symbol exist in the string
	Calculates the amount of memory words needed to hold the statment
	Returns: The calculated word count
*/
int getWordSpace(char*, int);


/*
	Input: a character to add to the data table
	Adds a single character to the data table
*/
void makeStrRow(int);


/*
	Input: a digit to add to the data table
	Adds a single digit to the data table
*/
void makeDataRow(char*);


/*	Creates a binary word for no-operand commands by calling makeBaseCommand
	Adds the created word to the instuction table by calling addCommandWord
*/
void third_inst_maker(char* );


/*	Creates a binary word for a single operand commands by calling makeBaseCommand and single_inst_maker
	Adds the created word to the instuction table by calling addCommandWord
*/
void second_inst_maker(char* , char*);


/*
	Takes the binary word, adds and binds in linked list with current IC 
*/
int addCommandWord(int, char*);


/*	
	Creates a binary word for a two operand commands by calling makeBaseCommand and single_inst_maker
	Adds the created word to the instuction table by calling addCommandWord
*/
int first_inst_maker(char*, char*, char*);


/*
	Input: operand word
	Creates a binary word relavant to the needed word,
	and adds it to the instruction table 
*/
void single_inst_maker(char*, int);


/* 
	Input: an entire statement  (without a label)
	Creates a row in the data table for	every character by calling makeStrRow.
*/
void makeStringInstruction(char*);


/* 
	Input: an entire statement  (without a label)
	Creates a row in the data table for	every digit by calling makeDataRow.
*/
void makeDataInstruction(char*);


/*
	Input: a filename without extension
	Returns: A FILE pointer 
*/
FILE* openFile(char*);


/*
	Calls the macro to free the memory on the three different tables
	Defined as a linked list implementation
*/
void freeMem();

