#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> //Para uso de flags
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

/*

La orden chmod modifica los permisos de archivos conforme al modo. Soporta dos modos distintos:

- Modo simbólico: argumento mnemotectico para espcificar los permisos.
Por ejemplo,  chmod ugo+rw ejemplo.txt  otorga permisos de escritura y lectura al usuario que posee
el archivo ejemplo.txt (u), a otros usuarios del mismo grupo (g) y a otros usuarios que no estan en el grupo
(o).

Por ejemplo,  chmod a+w ejemplo2.txt  da permisos de escritura del archivo ejemplo2.txt a todos los 
usuarios (a).

El simbolo + indica que se le conceden los permisos que se indiquen a continuacion, mientras que - significa
que se les retiran.


- Modo octal: usa la base octal para dar un numero que representa el patron de bits del campo mode del i-nodo
Usa un numero de 3 cifras en la que cada cifra representa a un tipo de usuario [usuario, grupo, otros], en 
ese orden. Con valores que varian desde 0 a 7. El valor introducido es la suma de 0, 1, 2 o 4. Donde
cada uno representa un tipo de permiso distinto. 
Por ejemplo, 7 indicaría todos los permisos posibles: suma de 1 (ejecutar), 2 (escribir) y 4(leer).
Por ejemplo, 6 indicaría los permisos de 2 (escribir) y 4 (leer).

Así, se puede escribir  chmod 640 ejemplo.txt  donde se le da permisos de 2 (escribir) y 4 (leer) al usuario
propietario del fichero; permisos de 4 (leer) a los usuarios del mismo grupo y 0 (sin permisos) al resto.

*/


void main(int argc, char* argv[]){
    int opt, isFile = 0, modeR=0, modeW =0;
    char* filename = malloc(sizeof(char*));

    if(argc <2){
        fprintf(stderr, "Not enough arguments.\n");
        exit(1);
    }

    while((opt= getopt(argc, argv, "f:rw"))!=-1){
        switch(opt){
            case 'f':
                filename = optarg;
                isFile = 1;
                break;

            case 'r':
                modeR = 1;
                break;

            case 'w':
                modeW = 1;
                break;
            
            default:
                break;

        }
    }

    if(isFile == 0){
        printf("File not received.\n");
        exit(1);
    }

    int fd=0;
    if(modeR == 1 && modeW ==1){
        fd=open(filename, O_RDWR|O_CREAT|O_TRUNC);
    }
    else if(modeR==1){
        fd=open(filename, O_RDONLY|O_CREAT|O_TRUNC);
    }
    else if(modeW ==1){
        fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC);
    }
    else{
        printf("At least one mode must be indicated.\n");
        exit(1);
    }

    if(fd==-1){
        fprintf(stderr, "File could not be opened.\n");
        exit(1);
    }

    //Se prueba a leer y a escribir
    char* buff = "HELL IS OTHER PEOPLE";
    char buff2[10];
    size_t wr = write(fd, buff, sizeof(buff));
    size_t rd = read(fd, buff2, sizeof(buff2));

    if(wr == -1){
        fprintf(stderr, "Error in write().\n");
    }

    if(rd == -1){
        fprintf(stderr, "Error in read().\n");
    }

    free(filename);

    return 0;
}