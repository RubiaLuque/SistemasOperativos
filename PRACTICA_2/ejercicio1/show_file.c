#include <stdio.h>
#include <stdlib.h>
#include <err.h>

/*
getc() y put() leem y escriben solo 1 byte. Hay que reemplazarlos con fread y fwrite.

*/

#define N 128

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int ret;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	//Código a reemplazar:

	/* Read file byte by byte */
	/*
	while ((c = getc(file)) != EOF) {
		 Print byte to stdout 
		ret=putc((unsigned char) c, stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	} */

	char buf[N]; //buffer en el que guardar lo leido

	//se lee hasta que se acabe el fichero
	while((ret = fread(buf, sizeof(char), N, file))){
		//Tratamiento de errores
		if(ret<=0) {
			perror("Error occurred.\n");
			exit(EXIT_FAILURE);
		}

		//Para entender mejor cómo va:
		//Aqui se comprueba si se ha leido menos caracteres que tamaño tiene el buffer
		if(ret < N) fwrite(buf, sizeof(char), ret, stdout);
		//Aqui se escribe todo el buffer entero
		else fwrite(buf, sizeof(char), N, stdout);
	}
	

	fclose(file);
	return 0;
}
