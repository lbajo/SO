#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int
main (int argc, char *argv[])
{
	char *tmpdir;
	char *username;
	int pid;

	tmpdir=getenv("TMPDIR");
	username=getenv("USER");
	pid=getpid();

	if (username==NULL){
		fprintf(stderr,"%s: Username not found \n",argv[0]);
		exit(1);

	}else{
		
		if (tmpdir==NULL){
			tmpdir="/tmp";
		}

		char route[strlen(tmpdir)+3+strlen(username)+sizeof(pid)];

		strcat(route,tmpdir);
		strcat(route,"/");
		strcat(route,username);
		strcat(route,".");

		char *p= malloc(sizeof(pid));
		sprintf(p,"%d",pid);

		strcat(route,p);

		printf("%s\n",route);
	}

	exit(0);
}