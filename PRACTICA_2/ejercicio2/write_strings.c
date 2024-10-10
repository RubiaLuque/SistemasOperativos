#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if(argc < 2){
		fprintf(stderr,"Usage: %s <file_name> <TEXT>\n",argv[0]);
		exit(1);
	}

	FILE* file;
	int ret;

	//Con el modo "rw" crea un archivo si no existe y lo trunca
	if ((file = fopen(argv[1], "w+")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	int i = 2;
	while(i<argc)  {
		
		//int seek = fseek(file, 0, SEEK_END); <-- No hace falta hacer esto si se abre el fichero con la opcion w+
		
		ret = fwrite(argv[i], sizeof(char), strlen(argv[i]), file);
		if(ret <= 0){
			perror("Error occurred.\n");
			exit(EXIT_FAILURE);
		}

		char * fin = "\0"; //IMPORTANTE PONER COMILLAS DOBLES PARA QUE LA DIRECCION DEL PUNTERO SEA NORMAL Y NO NULL.
		//DE OTRA MANERA SU DIRECCION SERIA 0x00 (null) Y DARIA VIOLACION DE SEGMENTO

		ret=fwrite(fin, sizeof(char), 1, file);
		if(ret <= 0){
			perror("Error occurred.\n");
			exit(EXIT_FAILURE);
		}

		++i;
	}

	return 0;
}
