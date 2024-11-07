#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOQ 512
/*
ANOTACIONES:
 - Si en la terminal se pone echo "text" > a.txt el texto se copiara dentro del archivo y no saldra por salida estandar.
 - Al compilar pueden dar warnings diciendo si se quiere usar fread o fwrite. No influyen en el comportamiento del programa. No peta ./copy
*/

void copy(int fdo, int fdd)
{
	char buffer[BLOQ];
	int ret;
	
	while((ret = read(fdo, buffer, BLOQ)) != 0){
		write(fdd, buffer, ret);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3){
		fprintf(stderr,"Usage: %s <file_name> <original_file> < copied_file>\n",argv[0]);
		exit(1);
	}

	//Descriptor de fichero (fd)
	int fdo = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);
	//Se abre como solo escritura, se crea si no existe y se trunca su contenido si es que tiene
	int fdd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP);

	if(fdo == -1 || fdd == -1){
		perror("No se pudo abrir un fichero");
		exit(EXIT_FAILURE);
	}

	copy(fdo, fdd);

	//MUY IMPORTANTE HACER ESTO
	close(fdo);
	close(fdd);

	return 0;
}
