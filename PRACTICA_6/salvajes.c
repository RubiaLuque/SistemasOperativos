#include <fcntl.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> //Uso memoria compartida
#include <sys/stat.h> //Para las constantes de mode_
#include <unistd.h>

#define NUMITER 3 //Cuántas veces intenta comer del caldero


// Este programa accede a los recursos compartidos
sem_t *empty;
sem_t *full;
int *pot;

//Solo si hay raciones suficientes en el caldero
int getServingsFromPot(void)
{
	(*pot) -= 1;
}

void eat(int servingsLeft)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	if(servingsLeft == 0){
		printf("Savage has eaten all of his servings\n");
	}
	else{
		printf("Remaining servings for %lu savage: %d", id, servingsLeft);
	}

	sleep(rand() % 5);
}

void savages(void)
{
	for (int i = 0; i < NUMITER; ++i)
	{
		sem_wait(full); //Espera a que el caldero se llene si hace falta
		getServingsFromPot(); //Se sirve si se cumple que el caldero no esta vacio
		printf("Servings left: %d\n", (*pot));

		if((*pot) == 0){
			sem_post(empty); //Se ha acabado el contenido del caldero
		}
		else{
			sem_post(full); //Sigue estando con comida suficiente para que el resto de salvajes se sirva
		}
		eat(NUMITER - i);
	}
}

int main(int argc, char *argv[])
{
	//Apertura de recursos compartidos
	//No hace falta darles valores a los semaforos porque ya lo hizo cocinero.c
	full = sem_open("/SEM_FULL", 0);
	empty = sem_open("/SEM_EMPTY", 0);
	int shd = shm_open("/POT", O_RDWR, 0);
	if(shd == -1){
		printf("Error in savages.c accessing shared memory\n");
		exit(EXIT_FAILURE);
	}
	pot = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shd, 0666);

	if(empty == SEM_FAILED || full == SEM_FAILED){
		printf("cocinero.c is not running. Close and run cocinero.c first\n");
		exit(EXIT_FAILURE);
	}

	savages();

	//Cierre de recursos
	sem_close(empty);
	sem_close(full);
	munmap(pot, sizeof(int));
	//shm_unlink("/POT"); // Se elimina la memoria compartida, pero solo debe hacerse en cocinero.c
	return 0;
}
