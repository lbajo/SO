// LORENA BAJO REBOLLO			TELEMÁTICA

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

int 
comprimir(int tam, char *nombre){
	int fd1,nr,w;
	int fd[2];
	char buffer[tam];
	int ret=0;

	fd1=open(nombre,O_CREAT|O_WRONLY|O_APPEND,0660);

	if (fd1<0){
		err(1,"Error creat");
	}

	if (pipe(fd)<0){
		err(1,"Pipe failed\n");
	}

	switch(fork()){
	case -1:
		err(1,"No se ha podido crear fork");
		break;

	case 0:
		if(close(fd[1])<0){
			err(1,"close \n");
		}
		dup2(fd[0],0);
		dup2(fd1,1);
		if(close(fd1)<0){
			err(1,"close \n");
		}
		if(close(fd[0])<0){
			err(1,"close \n");
		}
		execl("/bin/gzip","gzip","-",NULL);
		err(1,"error, gzip fallido");
		exit(1);
	default:
		if(close(fd[0])<0){
			err(1,"close \n");
		}
		nr=read(0, buffer, sizeof buffer);
		if (nr<tam){
			if (nr>1){
				w=write(fd[1],buffer,nr);
				if (w<0){
					err(1, "write");
				}
			}
			ret=-1;
		}else{
			w=write(fd[1],buffer,nr);
			if (w<0){
				err(1, "write");
			}
			ret=0;
		}

		if (close(fd1)<0){
			err(1,"close \n");
		}
	}
	return ret;
}

int 
main(int argc, char *argv[]){
	int t=atoi(argv[1]);
	int cond=0;
	int i=2;
	
	cond=comprimir(t,argv[i]);
	while(cond==0){
		i=i+1;
		if (i>=argc){
			i=2;
		}
		cond=comprimir(t,argv[i]);
	}

	exit(0);
}
