#!/bin/bash

touch main.c
echo "#include <stdlib.h>" > main.c
echo "int" >> main.c
echo "main(int argc,char *argv[]){" >> main.c
echo "
	exit(0);
}" >> main.c
