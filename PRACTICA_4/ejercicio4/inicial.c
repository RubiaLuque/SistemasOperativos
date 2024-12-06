#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/*
Nos dan el codigo inferior que contiene errores. Un problema es que el codigo no garantiza la ejecución ordenada
de procesos. Al usar lseek combinado con el problema anterior, obtenemos carreras.
Esto se soluciona no usando lseek para saber en qué posicion escribir, sino que hay que pausar el proceso
padre para que no cree más hilos hasta que no haya acabado el hijo de escribir.

int main(void)
{
    int fd1,fd2,i,pos;
    char c;
    char buffer[6];

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    write(fd1, "00000", 5);
    for (i=1; i < 10; i++) {
        pos = lseek(fd1, 0, SEEK_CUR);
        if (fork() == 0) {
            /* Child
            sprintf(buffer, "%d", i*11111);
            lseek(fd1, pos, SEEK_SET);
            write(fd1, buffer, 5);
            close(fd1);
            exit(0);
        } else {
            /* Parent
            lseek(fd1, 5, SEEK_CUR);
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}*/

int main(void)
{
    int fd1, i;
    char c;
    char buffer[6];

    //Se crea un archivo output.txt que se trunca si existe o se crea si no. Con permisos RW para el usuario
    // que lo crea
    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    //APARTADO A (siguiente linea). Comentar/descomentar para dicho apartado
    //write(fd1, "00000", 5);

    //Bucle en el que el proceso padre crea hijos. Solo avanza cuando ha acabado el proceso hijo en curso
    for (i = 1; i < 10; i++)
    {
        //APARTADO B (solo siguiente linea). Comentar/descomentaar para dicho apartado
        write(fd1, "00000", 5);
        if (fork() == 0)
        {
            /* Child */
            //Si el padre espera, no hace falta tener un metodo lseek que nos diga por donde vamos porque
            // eso ya lo hace el fd1
            sprintf(buffer, "%d", i*11111);
            write(fd1, buffer, 5);
            close(fd1); //Cierra el archivo para el proceso hijo pero no para todos
            exit(0); //Termina el proceso hijo

        } else {
            /* Parent */
            wait(NULL); //El proceso padre espera
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}

/*
SALIDA APARTADO A:
File contents are:
00000111112222233333444445555566666777778888899999

SALIDA APARTADO B:
File contents are:
000001111100000222220000033333000004444400000555550000066666000007777700000888880000099999
*/
