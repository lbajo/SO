all: compilar ejecutar

compilar:
		gcc -c -g -Wall -Wshadow shell.c
		gcc -o shell shell.o

ejecutar:
		./shell
clean:

	rm shell.o
	rm shell