// LORENA BAJO REBOLLO			TELEMÁTICA

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>


int
procesar_directorio(char *d,int p){
	DIR *direct;
	char *direct_interno;
	char *direct_total;
	char *direct_total2;
	char *dir_aux;
	char *dir_aux2;
	struct dirent *arch;
	int num_trash=0;
	int contador_errores=0;
	int sts;
	int n,n2;

	direct=opendir(d);

	if (direct==NULL){
		err(1,"No se puede abrir el directorio %s ",d);
	}

 	while ((arch= readdir(direct)) != NULL) {
 		if((strcmp(arch->d_name,".")!=0)&&(strcmp(arch->d_name,"..")!=0)){
			if (arch->d_type==DT_DIR){

				direct_interno=d;
				n=strlen(d);
				dir_aux=(char*)malloc(n*sizeof(char));

				dir_aux=strcat(dir_aux,direct_interno);
				dir_aux=strcat(dir_aux,"/");
				dir_aux=strcat(dir_aux,arch->d_name);

				contador_errores=procesar_directorio(dir_aux,p);

			}else if(strstr(arch->d_name,".trash")!=NULL){
				num_trash++;

				switch(fork()){
				case -1:
					err(1,"No se ha podido crear fork");
					break;
				case 0:
					direct_total=d;
					direct_total=strcat(direct_total,"/");
					direct_total=strcat(direct_total,arch->d_name);
					execl("/usr/bin/sha512sum","sha512sum",direct_total,NULL);
					contador_errores++;
					err(1,"%d: error sha512sum %s",p,arch->d_name);
					break;
				}

				while (wait(&sts)>0);
					
				switch(fork()){
				case -1:
					err(1,"No se ha podido crear fork");
					break;
				case 0:

					direct_total2=d;
					n2=strlen(d);
					dir_aux2=(char*)malloc(n2*sizeof(char));

					dir_aux2=strcpy(dir_aux2,direct_total2);
					dir_aux2=strcat(dir_aux2,"/");
					dir_aux2=strcat(dir_aux2,arch->d_name);
					
					execl("/bin/rm","rm",dir_aux2,NULL);
					contador_errores++;
					err(1,"%d: error removing %s\n",p,arch->d_name);
					break;
				}

				while (wait(&sts)>0);
			}
		}
 	}

 	if (num_trash ==0){
 		contador_errores++;
 		fprintf(stderr,"%d: no files to remove in dir %s\n",p,d);
 	}

	if (closedir(direct)==-1){
		err(1,"Error al cerrar el directorio %s ",d);
	}

	return contador_errores;
}


int 
main(int argc, char *argv[]){
	int pid,sts,errores;

	for(int i=1; i<argc; i++){
		pid=fork();
		switch(pid){
		case -1:
			err(1,"Fork ha fallado");
			break;
		case 0:
			pid=getpid();
			errores=procesar_directorio(argv[i],pid);
			if (errores!=0){
				fprintf(stderr,"%d processes failed\n",errores);
			}else{
				fprintf(stderr,"%d: %s ok\n",pid,argv[i] );
				fprintf(stderr,"all processes were successful\n");
			}

			exit(0);
			break;
		}
	}
	while (wait(&sts)>0);

	exit(0);
}
