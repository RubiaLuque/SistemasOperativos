#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

//Funcion auxiliar del ejercicio 2
char *loadstr(FILE *file)
{
	int cont = 0; // Tamaño de la palabra

	char c = 1;
	while (!feof(file) && c != '\0')
	{
		// Se lee byte a byte hasta que se acaba la palabra para saber su longitud
		if (fread(&c, sizeof(char), 1, file) == 1)
		{
			cont++;
		}
	}

	// Archivo vacio o final del archivo
	if (cont == 0)
	{
		return NULL;
	}
	// Crea la variable donde se guardara la palabra a leer
	char *string = (char *)malloc(sizeof(char) * cont);

	// Comprobacion de errores
	if (string == NULL)
		return NULL;

	// Se vuelve hacia atras el tamaño de la palabra
	fseek(file, -sizeof(char) * cont, SEEK_CUR);

	// Ahora ya sí, se lee la palabra entera
	fread(string, sizeof(char), cont, file);

	return string;
}

int print_text_file(char *path)
{
	//Se abre el archivo
	FILE *file = NULL;
	if ((file=fopen(path, "r+")) == NULL){
		fprintf(stderr, "File %s could not be opened\n", path);
		exit(EXIT_FAILURE);
	}

	//Se obtiene el numero de estudiantes de la primera linea
	char line[MAXLEN_LINE_FILE];
	fgets(line, MAXLEN_LINE_FILE*sizeof(char), file);

	int numStudents = atoi(line); //de string a int
	if(numStudents == 0){
		printf("Failed to read number of students\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	student_t student;
	char *token;
	int i = 0;
	// Se van leyendo los datos de los estudiantes separando la linea entera en tokens divididos por ':'
	while(fgets(line, MAXLEN_LINE_FILE*sizeof(char), file)!=NULL){
		char *lineptr = line;
		if ((token = strsep(&lineptr, ":")) != NULL)
			student.student_id = atoi(token);

		if ((token = strsep(&lineptr, ":")) != NULL)
			strcpy(student.NIF, token);

		if ((token = strsep(&lineptr, ":")) != NULL)
			student.first_name = token;

		if ((token = strsep(&lineptr, ":")) != NULL)
			student.last_name = token;

		printf("[Entry %d]\n", i);
		printf("\tstudent_id = %d\n", student.student_id);
		printf("\tNIF = %s", student.NIF);
		printf("\tFirst name = %s\n", student.first_name);
		printf("\tSecond name = %s\n", student.last_name);

		++i;
	}

	fclose(file);

	return 0;
}

int print_binary_file(char *path)
{
	FILE *file;
	if ((file = fopen(path, "rb")) == NULL)
	{
		fprintf(stderr, "File %s could not be opened: ", path);
		exit(EXIT_FAILURE);
	}

	int numStudents;
	fread(&numStudents, sizeof(int), 1, file);

	student_t student;

	int i = 0;
	for (i = 0; i < numStudents; ++i){
		// id del estudiante
		fread(&student.student_id, 4, 1, file);
		// no se puede igualar porque es un array estático, hay que copiar el valor
		char *NIF = loadstr(file);
		strcpy(student.NIF, NIF);
		char *firstName = loadstr(file);
		// se crea nueva memoria con strdup, por lo que no pasa nada por hacer el free antes o después de la escritura
		student.first_name = strdup(firstName);
		char *secondName = loadstr(file);
		student.last_name = strdup(secondName);

		free(NIF);
		free(firstName);
		free(secondName);

		printf("[Entry %d]\n", i);
		printf("\tstudent_id = %d\n", student.student_id);
		printf("\tNIF = %s\n", student.NIF);
		printf("\tFirst name = %s\n", student.first_name);
		printf("\tSecond name = %s\n", student.last_name);
	}

	fclose(file);
	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	// Se abre el archivo input
	FILE *iFile = NULL;
	if ((iFile = fopen(input_file, "r")) == NULL)
	{
		fprintf(stderr, "File %s could not be opened\n", input_file);
		exit(EXIT_FAILURE);
	}

	// Se abre el archivo de output
	FILE *oFile = NULL;
	if ((oFile = fopen(output_file, "wb")) == NULL)
	{
		fprintf(stderr, "File %s could not be opened\n", output_file);
		fclose(iFile);
		exit(EXIT_FAILURE);
	}

	// Se obtiene el numero de estudiantes de la primera linea
	char line[MAXLEN_LINE_FILE];
	fgets(line, MAXLEN_LINE_FILE * sizeof(char), iFile);
	int numStudents = atoi(line); // de string a int
	//Se escribe el numero de estudiantes en el archivo de output
	fwrite(&numStudents, sizeof(int), 4, oFile);

	student_t student;
	char *token;
	while (fgets(line, MAXLEN_LINE_FILE * sizeof(char), iFile) != NULL)
	{
		char *lineptr = line;
		if ((token = strsep(&lineptr, ":")) != NULL)
			student.student_id = atoi(token);

		if ((token = strsep(&lineptr, ":")) != NULL)
			strcpy(student.NIF, token);

		if ((token = strsep(&lineptr, ":")) != NULL)
			student.first_name = token;

		if ((token = strsep(&lineptr, ":")) != NULL)
			student.last_name = token;

		fwrite(&student.student_id, sizeof(int), 1, oFile);
		fwrite(student.NIF, sizeof(char), MAX_CHARS_NIF, oFile);
		fwrite(student.first_name, sizeof(char), strlen(student.first_name), oFile);
		fwrite(student.last_name, sizeof(char), strlen(student.last_name), oFile);
	}

	fclose(iFile);
	fclose(oFile);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -i <input_file> | -p | -o<output_file> | -b | -a ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;

		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;

		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = optarg;
			break;

		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
