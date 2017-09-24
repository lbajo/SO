// LORENA BAJO REBOLLO			TELEMÁTICA

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define N 1000

struct Comando {
	char *cmd;
	char *ruta;
	char *salida;
	char *entrada;
	char *args[N];
	char *total[N];
	int seg_plano;
};

typedef struct Comando Comando;
int fent;

Comando
new_cmnd(char linea[]){
	char *trozo=NULL;
	int count=0, count2=0, nargs=0;
	int p_e=-2, p_s=-2;

	Comando cmnd;

	cmnd.cmd=NULL;
	cmnd.ruta=NULL;
	cmnd.entrada=NULL;
	cmnd.salida=NULL;
	cmnd.seg_plano=0;
	memset(cmnd.args,0,sizeof cmnd.args);
	memset(cmnd.total,0,sizeof cmnd.total);

	trozo=strtok(linea," ");

	while(trozo!=NULL){
		if (count==0){
			if (strchr(trozo,'&')!=NULL){
				cmnd.seg_plano=1;	
			}else{
				cmnd.cmd=trozo;
			}
		}else{
			cmnd.args[nargs]=trozo;
			nargs++;
		}

		if (strchr(trozo,'>')!=NULL){
			p_s=count2+1;
		}else if(strchr(trozo,'<')!=NULL){
			p_e=count2+1;
		}

		if(p_s==count2){
			cmnd.salida=trozo;

		}else if(p_e==count2){
			cmnd.entrada=trozo;
		}else{
			if (strchr(trozo,'&')==NULL&&strchr(trozo,'>')==NULL&&strchr(trozo,'<')==NULL){
				cmnd.total[count]=trozo;
				count++;
			}
		}

		trozo=strtok(NULL," ");
		count2++;
	}
	return cmnd;
}

int
know_total(Comando cmnd){
	int total=0;

	for(int i=0;i<N;i++){
		if (cmnd.total[i]!=NULL){
			total++;
		}
	}
	return total;
}

void
cd(Comando cmnd){
	char *home=NULL;

	if(cmnd.args[0]!=NULL){
		if(chdir(cmnd.args[0])!=0){
			fprintf(stderr,"Directory doesn't exist or can't change directory\n");
		}
	}else{
		if((home=getenv("HOME"))==NULL){
			fprintf(stderr,"Error in getenv function\n");
		}
		if(chdir(home)!=0){
			fprintf(stderr,"Directory doesn't exist or can't change directory\n");
		}
	}
}

void
new_var(Comando cmnd){
	char *cadena, *var, *valor, *trozo, *linea, *aux = NULL;
	int count=0, tot, mem;

	tot=know_total(cmnd);
	aux=malloc(strlen(cmnd.cmd));
	if(aux==NULL){
		fprintf(stderr,"Memory error\n");
		exit(1);
	}
	linea=strcpy(aux,cmnd.cmd);
	trozo=strtok(linea,":=");

	while(trozo!=NULL){
		if (count==0){
			var=trozo;
		}
		else{
			valor=trozo;
		}
		trozo=strtok(NULL,":=");
		count++;
	}

	mem=strlen(var)+strlen(valor)+2;
	cadena=malloc(mem);
	if(cadena==NULL){
		fprintf(stderr,"Memory error\n");
		exit(1);
	}
		
	strcpy(cadena,var);
	strcat(cadena,"=");
	strcat(cadena,valor);

	if(putenv(cadena)!=0){
		fprintf(stderr,"Error in putenv function\n");
		free(cadena);
	}

	for(int i=0;i<tot;i++){
		if (cmnd.total[i]!=NULL){
			if(strcmp(cmnd.total[i],cmnd.cmd)==0){
				cmnd.total[i]=valor;
			}
		}
	}	
	cmnd.cmd=valor;
}


Comando
value(Comando cmnd){
	int j,k,tot=0;
	char palabra[N];
	char *aux, *linea, *pal=NULL;

	tot=know_total(cmnd);

	for(int i=0;i<tot;i++){
		if ((cmnd.total[i]!=NULL)&&(strchr(cmnd.total[i],'^')!=NULL)){
			j=0;
			k=0;

			aux=malloc(strlen(cmnd.total[i]));
			if(aux==NULL){
				fprintf(stderr,"Memory error\n");
				exit(1);
			}
			linea=strcpy(aux,cmnd.total[i]);

			while (linea[k]!='\0'){
				if (linea[k]!='^'){
					palabra[j]=linea[k];
					j++;
				}
				k++;
			}
			palabra[j]='\0';

			if((pal=getenv(palabra))==NULL){
				fprintf(stderr,"var %s does not exist\n",palabra);
			}
			for(int l=0;l<tot-1;l++){
				if(strcmp(cmnd.args[l],cmnd.total[i])==0){
					cmnd.args[l]=pal;
				}
			}

			cmnd.total[i]=pal;

			if((strchr(cmnd.cmd,'^')!=NULL)&&(i==0)){
				cmnd.cmd=pal;
			}
		}
	}
	return cmnd;
}

char* 
which(char *cmnd, char *cwd) {
	char *path, *ruta;
	char buf1[1024], buf2[1024];

	strcpy(buf2, cwd);
	strcat(buf2, "/");
	strcat(buf2, cmnd);

	if(access(buf2, F_OK) == 0) {
		ruta = buf2;
		return ruta;
	}else{
		strcpy(buf2, getenv("PATH"));
		path = strtok(buf2, ":");

		while(path!=NULL) {
			strcpy(buf1, path);
			strcat(buf1, "/");
			strcat(buf1, cmnd);
			if(access(buf1, F_OK) == 0) {
				break;
			}
			path = strtok(NULL, ":");
		}

		ruta = buf1;
		return ruta;
	}
	return ruta;
}

void 
execute(Comando cmnd, int num, int i, int first){
	int sts, fd[2], fd1, nr, w;
	char buf[1024];
	char buffer[N];

	getcwd(buf, sizeof(buf));
	cmnd.ruta=which(cmnd.cmd,buf);

	if (pipe(fd) < 0){
		err(1,"Pipe failed");
	}

	if (cmnd.salida != NULL){
		fd1=open(cmnd.salida,O_CREAT|O_WRONLY,0660);
		if(fd1==-1){
			err(1,"error open");
		}
			
	}else if(cmnd.entrada != NULL){
		fd1=open(cmnd.entrada,O_RDONLY);
		if(fd1==-1){
			err(1,"error open");
		}
	}

	switch(fork()){
	case -1:
		err(1,"Couldn't create fork");
		break;
	case 0:
		if (cmnd.salida != NULL){
			if(dup2(fd1, 1) < 0) {
				err(1, "error in dup2");
			}
		}
		if (cmnd.entrada != NULL){
			if(dup2(fd1, 0) < 0) {
				err(1, "error in dup2");
			}
		}
		if(first != 0) {
			if(dup2(fent, 0) < 0) {
				err(1, "error in dup2");
			}
		}
		if(i != num) {
			if(dup2(fd[1], 1) < 0) {
				err(1, "error in dup2");
			}
		} 
		execv(cmnd.ruta,cmnd.total);
		err(1,"error execv");
		break;
	}

	if (cmnd.salida!=NULL){
		while((nr=read(fd1,buffer, sizeof buffer)) > 0){
			if (nr < 0){
				w=write(fd[1],buffer,nr);
				if (w < 0){
					err(1, "write");
				}
			}
		}
	}

	while(wait(&sts)>0);
	fent=fd[0];
	if(close(fd[1])<0){
		err(1,"close \n");
	}
}

int
num_comands(char linea[]){
	int count=0;
	char *next=NULL;

	next=strtok(linea,"|");
	while(next!=NULL){
		count++;
		next=strtok(NULL,"|");
	}
	return count;
}

void
process_cmnd(char linea[], Comando cmnds[],int num){
	int count=0;
	char *next=NULL;
	char *list[num];

	next=strtok(linea,"|");

	while(next!=NULL){
		list[count]=next;
		next=strtok(NULL,"|");
		count++;
	}

	for(int i=0;i<count;i++){
		cmnds[i]=new_cmnd(list[i]);
	}

	if (count==0){
		cmnds[count]=new_cmnd(next);
	}
}

void 
detect(char linea[]){
	char linea_aux[N];
	int num_cmds=0;

	strcpy(linea_aux, linea);
	num_cmds=num_comands(linea_aux);
	Comando cmnds[num_cmds];
	process_cmnd(linea,cmnds,num_cmds);

	for(int i=0;i<num_cmds;i++){
		if (strcmp(cmnds[i].cmd,"cd")==0){
			cd(cmnds[i]);
		}else if(strstr(cmnds[i].cmd,":=")!=NULL){
			new_var(cmnds[i]);
		}	
	}
}

void
options(char linea[]){
	char linea_aux[N];
	int val=0,num_cmds=0,tot=0,first=0;

	strcpy(linea_aux, linea);
	num_cmds=num_comands(linea_aux);
	Comando cmnds[num_cmds];
	process_cmnd(linea,cmnds,num_cmds);

	for(int i=0;i<num_cmds;i++){
		if((strcmp(cmnds[i].cmd,"cd")!=0)&&(strstr(cmnds[i].cmd,":=")==NULL)){		
			tot=know_total(cmnds[i]);
			for(int j=0;j<tot;j++){
				if(cmnds[i].cmd!=NULL){
					if(strchr(cmnds[i].total[j],'^')!=NULL){
						val=1;
					}
				}
			} 
			if (i != 0){
				first=1;
			}
			if (val==0){
				execute(cmnds[i],num_cmds-1,i,first);
			}else{
				execute(value(cmnds[i]),num_cmds-1,i,first);
			}
		}
	}
}

int 
main(int argc, char *argv[]){
	char linea[N];
	char linea_aux[N];
	char linea_aux2[N];
	int dev,sts,pid,seg_plano=0;

	while(fgets(linea,N,stdin)!=NULL){
		if(strcmp(linea,"\n")!=0){
			strcpy(linea_aux, linea);
			strcpy(linea_aux2, linea);

			if(strchr(linea_aux2,'&')!=NULL){
				seg_plano=1;
			}

			detect(strtok(linea,"\n"));
			switch(pid=fork()){
			case -1:
				err(1,"Couldn't create fork");
				break;
			case 0:
				dev = open("/dev/null", O_RDONLY);
				if(dup2(dev, 0) < 0) {
					err(1, "error in dup2");
				}
				if(close(dev)<0){
					err(1,"close \n");
				}
				options(strtok(linea_aux,"\n"));
				exit(0);
				break;
			}

			if (seg_plano==1){
				waitpid(pid,&sts,WNOHANG);
			}else{
				while(wait(&sts)>0);
			}
			seg_plano=0;
		}
	}

	exit(0);

}