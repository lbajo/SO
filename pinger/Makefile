all: enlazar

main:
	gcc -c -g -Wall -Wshadow main.c

pinger:
	gcc -c -g -Wall -Wshadow pinger.c

enlazar: main pinger
	gcc -o pinger main.o pinger.o

clean:
	rm *.o

nuke: clean
	rm main 
