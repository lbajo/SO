//LORENA BAJO REBOLLO 	TELEMÁTICA

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_TABLA 26

struct Lista{
	char letra;
	int contador;
	int activado;
};

typedef struct Lista Lista;

Lista tabla[MAX_TABLA];

void 
inicializar(void){

	tabla[0].letra='a';
	tabla[1].letra='b';
	tabla[2].letra='c';
	tabla[3].letra='d';
	tabla[4].letra='e';
	tabla[5].letra='f';
	tabla[6].letra='g';
	tabla[7].letra='h';
	tabla[8].letra='i';
	tabla[9].letra='j';
	tabla[10].letra='k';
	tabla[11].letra='l';
	tabla[12].letra='m';
	tabla[13].letra='n';
	tabla[14].letra='o';
	tabla[15].letra='p';
	tabla[16].letra='q';
	tabla[17].letra='r';
	tabla[18].letra='s';
	tabla[19].letra='t';
	tabla[20].letra='u';
	tabla[21].letra='v';
	tabla[22].letra='w';
	tabla[23].letra='x';
	tabla[24].letra='y';
	tabla[25].letra='z';

	for(int i=0; i<26; i++){
		tabla[i].contador=0;
	}
	for(int i=0;i<26;i++){
		tabla[i].activado=0;
	}
}

void 
activar(char c){
	for(int i=0; i<MAX_TABLA; i++){
		if (c==tabla[i].letra){
			tabla[i].activado=1;
		}
	}
}

void 
contar(char c){
	for(int i=0; i<MAX_TABLA; i++){
		if (c==tabla[i].letra){
			tabla[i].contador+=1;
		}
	}
}

void 
buscar(char c, char buf[]){
	for (int i=0;i<strlen(buf);i++){
		if(c==tolower(buf[i])){
			contar(c);
		}
	}
}

void 
imprimir(void){
	for(int i=0; i<MAX_TABLA; i++){
		if (tabla[i].activado==1){
			printf("%c: %d\n",tabla[i].letra,tabla[i].contador);
		}
	}
}

int
main (int argc, char *argv[])
{
	char *fich;
	char buffer[1024];
	int nr,fd;
	char ch;

	inicializar();

	for(int i=0; i<argc; i++){
		if (strcmp(argv[i],"-f")==0){
			fich=argv[i+1];

			if(fich==NULL){
				err(1,"error al abrir el fichero \n");
			}
			break;

		}else{
			
			fich="terminal";
		}
	}

	if (strcmp(fich,"terminal")==0){

		fd=0;

		while((nr=read(fd,buffer, sizeof buffer))>0){

			if (nr<0){
				close(fd);
				err(1,"read %s\n",fich);

			}else if(nr==0){
				break;

			}else{
				for(int i=1; i<argc; i++){
					ch=tolower((char)*argv[i]);
					buscar(ch,buffer);
					activar(ch);
				}
			}

			memset(buffer,0,sizeof buffer);
		}

		imprimir();

	}else{

		fd=open(fich,O_RDONLY);

		if (fd<0){
			err(1,"open %s\n", fich);
		}

		while((nr=read(fd,buffer, sizeof buffer))>0){
			if (nr<0){
					close(fd);
					err(1,"read %s\n",fich);
			}

			for(int i=1; i<argc; i++){
				if (strcmp(argv[i],"-f")==0){
					i++;
				}else{
					ch=tolower((char)*argv[i]);
					buscar(ch,buffer);
					activar(ch);
				}
			}

			memset(buffer,0,sizeof buffer);
		}

		imprimir();

		if (close(fd)<0){
			err(1,"close %s\n",fich);
		}
	}
	exit(0);
}