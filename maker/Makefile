
all: dinam dinamsm

dinam: x.c main.c

	gcc -c -g -Wall -Wshadow *.c
	gcc -o dinam *.o

dinamsm: x.c main.c
	gcc -c -g -Wall -Wshadow *.c
	gcc -o dinamsm *.o 
	strip -s dinamsm

x.c:
	./newformat.sh *.h

main.c:
	./newmain.sh

clean:
	rm *.o

nuke: clean
	rm dinam 
	rm dinamsm
