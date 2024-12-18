#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> //Para uso de flags
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

//Programa con el mismo comportamiento que el comando cat.

int main(int argc, char *argv[])
{
	int opt, N = 0, end = 0;
	char* filename;

	if(argc <=1){
		fprintf(stderr, "Not enough arguments.\n");
		exit(1);
	}

	//opcion -n N indica el numero de bytes que queremos leer del final (si aparece -e tambien), o saltarnos
	// del principio (no aparece -e)
	filename = argv[optind]; //opntind es la posicion del siguiente elemento que se va a procesar en argv

	while((opt = getopt(argc, argv, "n:e")) != -1){
		switch(opt){
			case 'n':
				N = atoi(optarg);
				break;
			case 'e':
				end = 1;
				break;
		}

	}

	int fd = open(filename, O_RDWR);
	
	if(end){
		lseek(fd, (-N), SEEK_END);
	}
	else{
		lseek(fd, N, SEEK_SET);
	}

	size_t read_, write_;
	char buf[1];
	do{
		read_ = read(fd, buf, 1); //Se lee byte a byte
		if(read_ == -1){
			fprintf(stderr, "No se ha leido correctamente.\n");
			break;
		}

		write_ = write(1, buf, read_); //El fd = 1 indica que se escribe por salida estandar
		if(write_ == -1){
			fprintf(stderr, "No se ha escrito el archivo correctamente.\n");
			break;
		}

	}while(read_ != 0);

	printf("\n");

	close(fd);

	return 0;
}
