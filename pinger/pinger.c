#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 300

void 
fmt_down(char *nombre){
	int sts,n;

	char *nuevo_nombre;
	char *nombre_aux;

	n=strlen(nombre);
	nombre_aux=(char*)malloc(n*sizeof(char));

	nuevo_nombre=nombre;
	nombre_aux=strcat(nombre_aux,nuevo_nombre);
	nombre_aux=strcat(nombre_aux,".down");

	switch(fork()){
	case -1:
		err(1,"No se ha podido crear fork");
		break;
	case 0:
		execl("/bin/mv","mv",nombre,nombre_aux, NULL);
		err(1,"Error mv");
		exit(1);
	break;
	}
	while (wait(&sts)>0);
}

void
comprobar_ping(char *nombre,char *fichero){
	int fd[2];
	char *existe;
	char linea[N];
	int contador=0;


	if (pipe(fd)<0){
		err(1,"Pipe failed");
	}

	switch(fork()){
	case -1:
		err(1,"No se ha podido crear fork");
		break;
	case 0:

		if(close(fd[0])<0){
			err(1,"close \n");
		}
		dup2(fd[1],2);
		if(close(fd[1])<0){
			err(1,"close \n");
		}

		execl("/bin/ping","ping","-c","1",nombre, NULL);
		err(1,"Error ping");
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
		while(fgets(linea,N,stdin)!=NULL){
			if((existe=strstr(linea,"unknown host"))!=NULL){
				contador++;
			}
		}
	}

	if (contador!=0){
		fmt_down(fichero);
	}
}

void
procesar_fichero(char *fichero){
	FILE *fich;
	char linea[N];

	if(((fich=fopen(fichero,"r"))==NULL)){
		err(1,"Error open");
	}

	while(fgets(linea,N,fich)!=NULL){
		strtok(linea,"\n");
		if(strcmp(linea,"\n")!=0){
			comprobar_ping(linea,fichero);
		}
	}

	if (fclose(fich)!=0){
		err(1,"Error close");
	}
}
