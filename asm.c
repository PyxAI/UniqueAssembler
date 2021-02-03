#include "asm.h"

/* Create by: Omri Algazi  and Ofer Musai */


void firstRun(char*, FILE*);
void secondRun(char*, FILE*);
int statement_exists(char*);

int main(int argc, char* argv[])
{
	int i;
	if(argc == 1){
		printf("No files selected. Please pass filename as arguments\n");
	}

	for(i = 1; i < argc; i++){
	  char* fileName = argv[i];
		FILE * fd = openFile(fileName);
		firstRun(fileName, fd);
		fclose(fd);
	}
	return 0;
}

/* reset the global variable every file*/
void  resetGlobalVariable() {	
	sym_head = NULL;
	dat_head = NULL;
	in_head = NULL;
	DC = 0;
	IC = 0;
	hasError = 0;
	currentLine = 0;
}

/* 	The first pass on the input file.
	Scans the file, then finds and adds:
	labels (symbols), .data and .string entries, .extern entries.
	Calculates how much space each command will take, and updates IC accordingly.
	*/
void firstRun(char* fileName, FILE* fd) {
	int data_space = 0;
	int isSymbol, isData, isEntry, isExtern;
	char * statement = (char*)calloc(STATEMENT_SIZE, sizeof(char));	
	
	resetGlobalVariable();
	
	printf("Starting first scan for file %s\n", fileName);	
	currentLine = 0;	
	/*Allow to get 1 char more then the limit for cheking if the statement is exceed the statment limit*/
	while((fgets(statement, STATEMENT_SIZE + 1, fd))) { 

		currentLine++;
		if(strlen(statement) >= STATEMENT_SIZE){
			setError();
			printf("The statement is larger then %d characters \n", STATEMENT_SIZE - 1);
			continue;
		}		
		if (!statement_exists(statement)){
			continue;
		}		
	    isSymbol = checkIfSymbol(statement); 
	    isData = checkIfData(statement);
	    if(isData){
			  if(isSymbol){
				  addSymbol(statement, DC, 1, 0);
			  }
			  buildInstructionWord(statement);
				
	    }else {
			  isExtern = checkIfExtern(statement);
			  isEntry = checkIfEntry(statement);
			  if(isExtern || isEntry){
					if(isEntry){
						continue;
					}
		  		handleExternStatement(statement);
	
			  }else {
					if(isSymbol){
						addSymbol(statement, IC+ START_IC, 0, 0); 
					}
					data_space = getWordSpace(statement, isSymbol);
					IC+=data_space;
			  }
	    
	    }
		if (IC + DC > MEMORY_SIZE){
			printf("Memory capcity excceeded allocated %d:\nToo many instructions or data items.\nexiting..\n", MEMORY_SIZE);
			exit(0);
		}
	}
	updateDataSymbolTable();
	rewind(fd);
	secondRun(fileName, fd);
}

/* 	The second pass on the input file.
	Now we are creating the words, verifying entries, building the files and freeing the memory
*/
void secondRun(char* fileName, FILE* fd) {
	int isData, isExtern, isEntry;
	char * statement = (char*)calloc(STATEMENT_SIZE, sizeof(char));	
	printf("Starting second scan for file %s\n", fileName);	
	IC = 0;
	currentLine = 0;
	while(fgets(statement, STATEMENT_SIZE, fd)) { /* Iterating the file */
		currentLine++;
		if (!statement_exists(statement)){ /* Is it an empty line ? */
			continue;
		}
		isData = checkIfData(statement);
		isExtern = checkIfExtern(statement);
		if(isData || isExtern){
			continue;
		}
		isEntry = checkIfEntry(statement);
		if(isEntry){
			handleEntryStatement(statement);
		}else{
			buildCommandWord(statement);
		}
	}
	if(!hasError){
		buildFiles(fileName);
	}
	freeMem();
	printf("Found a total of %d errors in %s.asm\n", hasError, fileName);
}

/*Input is a line statement
The function checks whether there is anything but whitespaces and ';' characters in the line
*/
int statement_exists(char* line)
{
	int ch, i=0;
	while (line[i]!=0){
		ch = line[i++];
		if (ch==';')
			return 0;
		if (ch==' ' || ch=='\t' || ch=='\n')
				continue;
		else
			return 1;
	}
		return 0;
}

