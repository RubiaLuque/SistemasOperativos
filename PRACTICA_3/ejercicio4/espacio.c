#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h> //Para abrir directorios
#include <string.h>

#define TAM_BLOQUE 512

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
	struct stat st;
	if(lstat(fname, &st)==-1){
		perror("lstat");
		exit(EXIT_FAILURE);
	}
	
	(*blocks) += st.st_blocks; 

	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
	//Se abre un directorio igual que ccon FILE*
	DIR* dir=opendir(dname);

	//Directorio no existente
	if(dir == NULL){
		printf("El directorio %s no se pudo abrir.\n", dname);
		closedir(dir);
		exit(EXIT_FAILURE);
	}

	struct dirent* dirst;

	while((dirst=readdir(dir))!=NULL){
		//Se comprueba si no se trata del directorio actual o de su padre porque estas entradas se ignoran
		// para no tener una recursion infinita
		if(strcmp(dirst->d_name, ".") != 0 && strcmp(dirst->d_name, "..")!=0){
			//Si se trata de un fichero regular
			if(dirst->d_type == DT_REG){
				get_size(dirst->d_name, blocks);
			}
			//Si se trata de otro directorio
			else if(dirst->d_type == DT_DIR){
				get_size_dir(dirst->d_name, blocks);
			}
		}

	}
	closedir(dir);

	return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{

	if(argc<=1){
		fprintf(stderr, "Faltan parámetros.\n");
		exit(1);
	}

	char* files[argc-1];

	size_t blocks = 0; //numero de bloques que ocupa un fichero

	struct stat st; //almacena info sobre el estado de los ficheros

	//Se guardan todos los ficheros en un array de strings
	for(int i = 1; i<argc;i++){
		files[i-1]=argv[i];
	}

	for(int i = 0; i<argc-1; i++){
		blocks = 0;

		//error en lstat
		if(lstat(files[i], &st) == -1){
			perror("lstat");
			exit(1);
		}

		//Se comprueba si un fichero es un directorio o un archivo regular
		if(S_ISREG(st.st_mode)){
			get_size(files[i], &blocks);
		}
		else if(S_ISDIR(st.st_mode)){
			get_size_dir(files[i], &blocks);
		}
		else{
			printf("%s is not a file or a directory.\n", files[i]);
		}

		//Cada bloque son 512 bytes
		printf("%s --- %d KB\n", files[i], (blocks*512)/1024);
	}

	return 0;
}
