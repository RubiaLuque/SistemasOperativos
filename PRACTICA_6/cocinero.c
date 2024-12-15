#define _XOPEN_SOURCE 500 // Para sigaction

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> //Para las flags
#include <sys/wait.h>
#include <semaphore.h> //Uso de semaforos
#include <pthread.h> //Uso de hilos
#include <sys/mman.h> //Uso memoria compartida
#include <sys/stat.h> //Para las constantes de mode_t
#include <signal.h> //Para el uso de señales

#define M 10 //Raciones totales

int finish = 0;

int* pot; //Debe ser un puntero para poder formar parte de la memoria compartida

sem_t* empty;
sem_t* full;


// Este programa crea los recursos compartidos

// Solo cuando el caldero esta vacio
void putServingsInPot(int servings)
{
	(*pot) = servings;
}

void cook(void)
{
	while (!finish)
	{
		sem_wait(empty); //Espera a que los salvajes notifiquen que no hay comida
		putServingsInPot(M); //Rellena el caldero
		sem_post(full); //Avisa que ya se puede comer
		printf("Pot full with %d rations\n", M);
	}
}

void handler(int signo)
{
	sem_close(empty); //Se cierran los semaforos
	sem_close(full);
	sem_unlink("/SEM_EMPTY"); //Se eliminan los semaforos con su nombre
	sem_unlink("/SEM_FULL");
	finish = 1;
	munmap(pot, sizeof(int)); //Se desmapea la mem compartida
	shm_unlink("/POT"); //Se elimina la memoria compartida
}

int main(int argc, char *argv[])
{
	//Semaforos con nombre y memoria compartida
	empty = sem_open("/SEM_EMPTY", O_CREAT, 0666, 1); //Valor inicial a 1
	full = sem_open("/SEM_FULL", O_CREAT, 0666, 0); //Valor inicial a 0

	if(empty == SEM_FAILED || full == SEM_FAILED){
		printf("sem_open failed in cocinero.c\n");
		exit(EXIT_FAILURE);
	}

	//Objeto a proyectar en memoria compartida. shd actua como un descriptor de fichero de la mem compartida
	int shd = shm_open("/POT", O_CREAT | O_RDWR, 0666);
	ftruncate(shd, sizeof(int)); //Se trunca la memoria compartida para que sea del tamaño de un int

	//(address de memoria, tamaño, flags de proteccion, flags de visibilidad, descriptor de mem compartida,
	// offset)
	// Si address de memoria esta a NULL, el kernel decide el sitio donde ponerlo, es la mejor opcion
	pot = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shd, 0);
	*pot = M;

	//MANEJO DE SEÑALES --> Si se reciben las señales de SIGINT o SIGTERM (cierre abrupto del programa)
	// el handler se encarga de cerrar los datos
	struct sigaction actions;
	actions.sa_handler = handler;
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = SA_RESTART;

	sigaction(SIGINT, &actions, NULL);
	sigaction(SIGTERM, &actions, NULL);

	//Se ejecuta el programa
	cook();


	//Si acaba el programa, se cierran los recursos usados
	sem_close(empty); // Se cierran los semaforos
	sem_close(full);
	sem_unlink("/SEM_EMPTY"); // Se eliminan los semaforos con su nombre
	sem_unlink("/SEM_FULL");
	munmap(pot, sizeof(int)); // Se desmapea la mem compartida
	shm_unlink("/POT");		  // Se elimina la memoria compartida

	return 0;
}
