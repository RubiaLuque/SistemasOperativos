#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
//Para definir las flags de tipos de ficheros. Aunque aparezca en rojo no da error de compilacion
#include <sys/sysmacros.h>
#include <time.h>

#define BLOQ 512
/*
ln -s <archivo> crea un enlace simbolico al archivo referido (con su ruta de acceso si es necesario)

Se hace:

ln -s ../ejercicio1/Makefile myLink
ls -l

Obtenemos:
total 12
-rwxr-xr-x 1 usuario_local users 1265 ago 31  2023 check_copy2
-rw-r--r-- 1 usuario_local users  216 ago 31  2023 copy2.c
-rw-r--r-- 1 usuario_local users  259 ago 31  2023 Makefile
lrwxrwxrwx 1 usuario_local users   22 oct 17 12:38 myLink -> ../ejercicio1/Makefile

Se intenta usar el ejercicio 1 (./copy) para copiar el enlace simbolico creado:
../ejercicio1/copy myLink myLinkCopy

Obtenemos un archivo Makefile igual al del ejercicio 1, pero al realizar cambios en el mismo, no se cambia ni en myLink ni en el Makefile original. Lo que
significa que solo se ha hecho una copia del contenido y no del enlace en sí. MyLinkCopy es un fichero regular

*/
void copy(int fdo, int fdd)
{
	char buffer[BLOQ];
	int ret;

	while((ret = read(fdo, buffer, BLOQ)) != 0){
		write(fdd, buffer, ret);
	}
}

void copy_regular(char *orig, char *dest)
{
	//Descriptor de fichero (fd)
	int fdo = open(orig, O_RDONLY, S_IRUSR | S_IRGRP);
	//Se abre como solo escritura, se crea si no existe y se trunca su contenido si es que tiene
	int fdd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP);

	if(fdo == -1 || fdd == -1){
		perror("No se pudo abrir un fichero");
		exit(EXIT_FAILURE);
	}

	copy(fdo, fdd);

	//MUY IMPORTANTE HACER ESTO
	close(fdo);
	close(fdd);

}

void copy_link(char *orig, char *dest)
{
	struct stat origStat;
	// Se reserva espacio para guardar el link que almacena el fichero apuntador
	char *aux = (char *)malloc(origStat.st_size + 1); //+1 porque lstat no incluye el caracter terminador

	// Se lee el link del fichero apuntador
	readlink(orig, aux, sizeof(aux));

	aux[origStat.st_size] = '\0'; // Se le añade a mano el caracter terminador

	// Se crea un enlace simbolico al archivo apuntado por el fichero original
	int sym = symlink(aux, dest);

	if (sym < 0)
	{
		fprintf(stderr, "Symbolic link could not be created.\n");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3){
		fprintf(stderr,"Usage: %s <file_name> <original_file> < copied_file>\n",argv[0]);
		exit(1);
	}



	struct stat* statbuf; //Donde se guardaran los datos
	int ret = lstat(argv[1], statbuf);

	if(ret == -1){
		perror("lstat failed\n");
		exit(EXIT_FAILURE);
	}

	switch(statbuf->st_mode & S_IFMT){
		case S_IFLNK: copy_link(argv[1], argv[2]); break;
		case S_IFREG: copy_regular(argv[1], argv[2]); break;
		default:
	}


	return 0;
}
