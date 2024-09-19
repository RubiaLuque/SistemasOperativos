#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> //Uso de open, close, lseek, etc y sus flags


int main(int argc, char* argv[]){
    //Mal paso de argumentos
    if(argc!=3)
    {
        printf("Uso: %s <fichero_in> <fichero_out>", argv[0]);
        exit(0);
    }

    //Argumentos bien pasados:

    int input_fd, output_fd; //Descriptores de ficheros
    int file_size;
    /*
    - Descriptor 0 --> entrada estandar
    - Descriptor 1 --> salida estandar
    - Descriptor 2 --> salida de error
    */

    //Abrir el fichero
    input_fd = open(argv[1], O_RDONLY);

    file_size = lseek(input_fd, 0, SEEK_END); //Se obtiene el tamaño del fichero obteniendo cuánto se ha movido el puntero 
    //del fichero (el descriptor)
    //La funcion anterior deja el puntero del fichero al final, habria que recolocarlo para no empezar a leer desde ahi

    char* buffer = malloc(file_size * sizeof(char)); //Se reserva memoria para un buffer de chars (es como hacer un new)

    //Se vuelve a colocar el descriptor del fichero al comienzo
    lseek(input_fd, 0, SEEK_SET); //Posicion 0 absoluta del fichero: el primer caracter

    //Si no se sabe qué cantidad de bytes se van a leer, read devuelve el valor del numero de bytes leídos
    int bytes_read = read(input_fd, buffer, file_size); //Mete en el buffer lo que ha leido
    printf("Se han leido %d bytes\n", bytes_read);

    output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    /**
     * Abre el fichero de manera que solo sea de escritura, si no existe lo crea, y si tiene contenido lo borra/lo trunca.
     * Además, da permisos de lectura (R) y escritura (W) al usuario que ha creado el fichero.
    */

    write(output_fd, buffer, bytes_read); //Se podria usar en este caso file_size porque sabemos que es igual que bytes_read, pero no 
    // siempre tiene por que ser asi


    //Cierre de recursos final 
    free(buffer);
    close(input_fd);
    close(output_fd);

    return 0;
}