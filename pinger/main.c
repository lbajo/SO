#include <stdlib.h>
#include <stdio.h>
#include "pinger.h"
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int 
main(int argc, char *argv[]){
	int pid,sts;

	for(int i=1; i<argc; i++){
		switch(pid=fork()){
		case -1:
			err(1,"Couldn't create fork");
			break;
		case 0:
			procesar_fichero(argv[i]);
			exit(0);
			break;
		}
		waitpid(pid,&sts,WNOHANG);
	}
	while (wait(&sts)>0);
	exit(0);
}
