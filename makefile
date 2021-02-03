asm : asm.o file.o word.o utils.o buildBinary.o
	gcc -g asm.o file.o word.o utils.o buildBinary.o -o asm -lm
asm.o : asm.c asm.h
	gcc -c -ansi -Wall -pedantic asm.c -o asm.o
file.o : file.c asm.h
	gcc -c -ansi -Wall -pedantic file.c -o file.o
word.o : word.c asm.h
	gcc -c -ansi -Wall -pedantic word.c -o word.o
utils.o : utils.c asm.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o
buildBinary.o : buildBinary.c asm.h
	gcc -c -ansi -Wall -pedantic buildBinary.c -o buildBinary.o