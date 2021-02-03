#include "asm.h"
#define OCTAL_LIMIT 5

/*
	Input: a filename without extension
	Returns: A FILE pointer 
*/
FILE* openFile(char* fileName) {

	FILE* fd;
	char name[FILE_NAME_LEN];
	strcpy(name, fileName);
	strcat(name, ".as");
	fd = fopen(name, "r");

	if(!fd){
		printf("Error: can't open the file: %s \n", fileName);
		exit(0);
	}
	
	return fd;
}

/* print word to file in octal format and max length of 5 */
void printOctalWord(FILE* fd, int word) {
	char text[FILE_NAME_LEN];
	int i;
	sprintf(text, "%.5o", word);

	for(i= strlen(text) - OCTAL_LIMIT; i < strlen(text); i++){ /* Write to file */
			fprintf(fd,"%c", text[i]); 
	}
}


/* 	
	Input: filename to create
	Output: creates an object file
	First writes the instructions, then the data.
*/
void createHashFile(char* fileName) {
	hashPtr row;
	FILE* fd;
	char name[FILE_NAME_LEN];
	strcpy(name, fileName);
	strcat(name, ".ob");
	fd = fopen(name, "w");
	if(!fd){
		printf("Error: cant create the file: %s \n", fileName);
		exit(0);
	}

	/* Writes initial lines */
	fprintf(fd,"%d %d \n",IC, DC); 
	row = in_head;

	/*Iterate instructions table */
	while (row != NULL){
	  fprintf(fd,"%04d ",row->address); 
	  printOctalWord(fd, row->word);
	  fprintf(fd,"\n"); 
	  
    row = row -> next;
	}
	/*Iterate data table */
	row = dat_head;
	while (row != NULL){
    fprintf(fd,"%04d ",row->address); 
	  printOctalWord(fd, row->word);
	  fprintf(fd,"\n"); 
	  
    row = row -> next;
	}
	
	fclose(fd);
}


/* 	
	creates an external file
	Input: filename to create
	Output: An .ext file
*/
void createExternalFile(char* fileName) {
	hashPtr codeRow;
	tablePtr row;
	long position;
	FILE* fd;
	char name[FILE_NAME_LEN];
	strcpy(name, fileName);
	strcat(name,".ext");
	fd = fopen(name, "w");
	
	if(!fd){
		printf("Error: cant create the file: %s \n", fileName);
		exit(0);
	}
	
	/* Iterate symbols table to find external symbols */
	row = sym_head;
	while (row != NULL){
		if(row->isExternal)
		{
			codeRow = in_head;
			while (codeRow != NULL){		
				if(!strcmp(codeRow->oprandName, row->name)){  
					fprintf(fd,"%s %04d \n",row->name, codeRow->address);
				}
					codeRow = codeRow -> next;
			}
		}
		row = row -> next;
	}
	position = ftell(fd);	
	fclose(fd);

	/*  delete empty file */
	if(!position){
		remove(name);
	}
		
}


/* 	
	create entry file
	Input: filename to create
	Output: An .ent file
*/
void createEntryFile(char* fileName) {
	tablePtr row;
	long position;
	FILE* fd;
	char name[FILE_NAME_LEN];
	strcpy(name, fileName);
	strcat(name, ".ent");
	fd = fopen(name, "w");
	if(!fd){
		printf("Error: cant create the file: %s \n", fileName);
		exit(0);
	}
	
	row = sym_head;
	/* Iterate symbols table to find entry symbols */
	while (row != NULL){
	    if(row->isEntry){
		    fprintf(fd,"%s %d \n",row->name, row->address);
	    }
    	row = row -> next;
	}

	position = ftell(fd);	
	fclose(fd);

	/* delete empty file */
	if(!position){
		remove(name);
	}
}


/*
	Calls the three functions for building our output files independently 
*/
void buildFiles(char* fileName){
	createHashFile(fileName);
	createEntryFile(fileName);
	createExternalFile(fileName);
}

