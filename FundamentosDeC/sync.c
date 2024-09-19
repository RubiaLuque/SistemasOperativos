#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter = 0, loops = 0;

void * worker(void* args){
    for(int i =0; i< loops; ++i){
        //Comienzo proteccion de memoria
        counter++;
        //Final proteccion de memoria
    }
}

int main(int argc, char* argv[]){
    loops = atoi(argv[1]); //Se castea de char* a int
    pthread_t p1, p2; //Dos identificadores de hilos. pthread_t es un struct

    //Crea dos hilos y se empiezan a ejecutar
    pthread_create(&p1, NULL, worker, NULL);
    pthread_create(&p1, NULL, worker, NULL);

    //Se detienen los hilos y se borran
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    
}

/*Los dos hilos estan operando sobre la misma direccion de memoria, que es counter. Como solo se puede ejecutar un hilo a la vez al 
escribir memoria, es posible que el otro al entrar despues no se haya actualizado el counter y escoja un valor anterior. Es por esto
que se necesita una sincronizacion y proteger la seccion de memoria compartida para que solo pueda entrar uno cada vez y se pueda 
actualizar la memoria antes de que pueda entrar el otro. Si no hay una sincronizacion explícita, el planificador puede dar paso a uno u 
otro indistintamente y se pueden crear problemas como el descrito.

*/
