#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	int cont = 0; //Tamaño de la palabra

	char c = 1;
	while(!feof(file) && c != '\0'){
		//Se lee byte a byte hasta que se acaba la palabra para saber su longitud
		if(fread(&c, sizeof(char), 1, file) == 1){
			cont++;
		}
	}

	//Archivo vacio o final del archivo
	if(cont == 0){
		return NULL;
	}
	//Crea la variable donde se guardara la palabra a leer
	char *string = (char *)malloc(sizeof(char) * cont);

	//Comprobacion de errores
	if(string == NULL)
		return NULL;

	//Se vuelve hacia atras el tamaño de la palabra
	fseek(file, -sizeof(char) * cont, SEEK_CUR);

	//Ahora ya sí, se lee la palabra entera
	fread(string, sizeof(char), cont, file);

	return string;
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "Usage: %s <file_name> <TEXT>\n", argv[0]);
		exit(1);
	}

	FILE *file = NULL;
	if((file=fopen(argv[1], "r+")) == NULL)
	{
		fprintf(stderr, "File could not be opened.\n");
		exit(EXIT_FAILURE);
	}

	while(!feof(file)){
		char *string = loadstr(file);
		if(string!=NULL){
			printf("%s\n", string);
			free(string); //Libera la memoria asignada por malloc
		}
	}

	return 0;
}
