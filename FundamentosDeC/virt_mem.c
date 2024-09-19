#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int * p = malloc(sizeof(int));
    printf("Address: %p\n", p);
    *p = 0;

    while(1){
        (*p)++; //Se aumenta el valor del puntero
        sleep(1);

        printf("[%d] Valor de p: %d \n", getpid(), *p); //getpid obtiene el id numerico del proceso
    }
}