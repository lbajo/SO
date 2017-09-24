// LORENA BAJO REBOLLO			TELEMÁTICA

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

int pidact;

void eliminarsinnombre(int pid){
	int sts;

	if (pid!=pidact && pid!=0){
		switch(fork()){
		case -1:
			err(1,"No se ha podido crear fork");
			break;
		case 0:
			if (kill(pid,9)==-1){
				err(1,"Error kill: %d",pid);
			}
			exit(0);
			break;
		}
		while (wait(&sts)>0);
	}
}

void
procesarlinea(char *linea){
	int pid;
	
	strtok(linea," ");
	pid=atoi(linea);
	eliminarsinnombre(pid);
}

void 
eliminarconnombre(char *nombre){
	int fd[2];
	char *existe;
	char linea[300];


	if (pipe(fd)<0){
		err(1,"Pipe failed\n");
	}

	switch(fork()){
	case -1:
		err(1,"No se ha podido crear fork");
		break;
	case 0:
	
		if(close(fd[0])<0){
			err(1,"close \n");
		}
		dup2(fd[1],1);
		if(close(fd[1])<0){
			err(1,"close \n");
		}
		execl("/bin/ps","ps","ax", NULL);
		err(1,"Error ps");
		exit(1);
		break;

	default:
		if(close(fd[1])<0){
			err(1,"close \n");
		}
		dup2(fd[0],0);

		if(close(fd[0])<0){
			err(1,"close \n");
		}
		while(fgets(linea,300,stdin)!=NULL){
			if((existe=strstr(linea,nombre))!=NULL){
				procesarlinea(linea);
			}
		}
	}
}

int 
main(int argc, char *argv[]){

	pidact=getpid();

	if (strcmp(argv[1],"-n")==0){
		for(int i=2; i<argc; i++){
			eliminarconnombre(argv[i]);
		}
	}else{
		for(int i=1; i<argc; i++){
			eliminarsinnombre(atoi(argv[i]));
		}	
	}
	exit(0);
}
