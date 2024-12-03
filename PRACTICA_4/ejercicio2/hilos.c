#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> //Para el uso y manejo de hilos
#include <unistd.h>
#include <fcntl.h> 

struct thread_info {
	int num_thread;
	char priority;
};


//El argumento puntero a void es una especie de placeholder que espera cualquier tipo de puntero
//Esta funcion obtiene los argumentos y los imprime. Es la funcion que van a ejecutar todos los hilos
void *thread_usuario(void *arg)
{
	int num = ((struct thread_info*)arg)->num_thread;
	int prior = ((struct thread_info*)arg)->priority;

	printf("Id: %ld\nThread number: %d\nPrioryty: %c\n\n", pthread_self(), num, prior);

	free(arg); //IMPORTANTE: liberar la memoria dinamica aqui
	//La funcion pthread_self da el identificador del hilo actual
}

int main(int argc, char* argv[])
{
	int numHilos = 15;
	pthread_t thread_id[numHilos]; //Identificador de hilo
	pthread_attr_t attr; //Atributos de un hilo
	void *res; //Puntero a nulo donde se almacena el estado de salida de un hilo

	pthread_attr_init(&attr); //Se obtienen los atributos inicializados por defecto en la variable attr

	//Se crean los hilos:
	for(int i = 0; i<numHilos; ++i){
		//Memoria dinamica asignada
		struct thread_info* info = malloc(sizeof(struct thead_info*));
		
		info->num_thread = i;

		//Numeros de hilos pares son prioritarios (P) e impares no prioritarios (N)
		info->priority = (info->num_thread %2 == 0) ? 'P':'N';

		//Con lo que ya tenemos podemos crear el hilo. Debemos hacer uso de la funcion thread_usuario,
		// ya que se usa como parametro de la funcion thread_create. Tras introducir ducha funcion como 
		// parametro, se deben pasar los suyos a continuación. La funcion que se pase como parametro a 
		// pthread_create es la que va a ejecutar el hilo
		int hilo = pthread_create(&thread_id[i], &attr, thread_usuario, info);

		if(hilo!=0){
			perror("pthread_create");
			free(info);
			exit(EXIT_FAILURE);
		}

		
	}

	//Espera a que acaben todos los hilos para borrarlos
	for(int i = 0; i<numHilos; i++){
		pthread_join(thread_id[i], &res);
	}

	return 0;
}

/*
Sucede que los hilos se crean todos con la misma prioridad y, por tanto, no hay un orden de ejecucion 
garantizado. El planificador los trata a todos por igual. Para hacer que se ejecuten primero los hilos
con prioridad P, habría que hacer cambios en el planificador para que los considere antes.

SALIDA:

Id: 140689014507200
Thread number: 1
Prioryty: N

Id: 140689022899904
Thread number: 0
Prioryty: P

Id: 140688997721792
Thread number: 3
Prioryty: N

Id: 140689006114496
Thread number: 2
Prioryty: P

Id: 140688989329088
Thread number: 4
Prioryty: P

Id: 140688980936384
Thread number: 5
Prioryty: N

Id: 140688700208832
Thread number: 6
Prioryty: P

Id: 140688691816128
Thread number: 7
Prioryty: N

Id: 140688683423424
Thread number: 8
Prioryty: P

Id: 140688675030720
Thread number: 9
Prioryty: N

Id: 140688666638016
Thread number: 10
Prioryty: P

Id: 140688658245312
Thread number: 11
Prioryty: N

Id: 140688649852608
Thread number: 12
Prioryty: P

Id: 140688641459904
Thread number: 13
Prioryty: N

Id: 140688633067200
Thread number: 14
Prioryty: P

*/


 