#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

/*
De los apuntes de Pascal:

- Hilos (Threads): Un hilo es una unidad de ejecución dentro de un proceso. Todos los hilos de un proceso comparten
el mismo espacio de direcciones.

- Mutex (Mutual Exclusion): Un mecanismo para prevenir que múltiples hilos accedan simultáneamente a una sección
crítica del código, evitando condiciones de carrera.

- Variables de Condición: Permiten que los hilos esperen a que una condición específica ocurra antes de continuar su
ejecución. Se debe cumplir en el código que wait siempre este en un bucle while, wait siempre tiene el mutex cogido
y signal mejor con el mutex cogido.
	- Signal permite desbloquear al menos a un hilo bloqueado por la variable de condicion
	- Broadcast permite desbloquearlos a todos los hilos bloqueados por la variable de condicion


*/


struct client{
	int id;
	int is_vip;
};

//Cola de clientes
struct queue{
	int next;
	int actual;
};

//Mutex y variable de condicion
pthread_mutex_t mutex_capacity;
pthread_cond_t cond_capacity;

//Variables de control globales
int occupation = 0;
struct queue vips = {0, 0};
struct queue no_vips = {0, 0};

void enter_normal_client(int id)
{
	pthread_mutex_lock(&mutex_capacity);
	//SECCION CRITICA
	int order = no_vips.next;
	++no_vips.next; //Se aumenta en 1 el turno siguiente
	printf("Client %2d NO VIP arrives at the queue in position %d\n", id, order);

	while(occupation >= CAPACITY || order != no_vips.actual || vips.actual != vips.next){
		printf("Client %2d NO VIP IS WAITING in queue.\n", id);
		pthread_cond_wait(&cond_capacity, &mutex_capacity);
	}

	printf("Client %2d NO VIP enters the disco.\n", id);
	++occupation;
	++no_vips.actual;
	printf("Actual occupation %d\n", occupation);

	pthread_mutex_unlock(&mutex_capacity);
}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex_capacity); //Bloquea el mutex a otros hilos
	//SECCION CRITICA
	int order = vips.next;
	++vips.next;
	printf("The client %2d VIP arrives at queue in position %d\n", id, order);

	while(occupation >= CAPACITY || order != vips.actual){
		printf("The client %2d IS WAITING in queue\n", id);
		//Se espera a que se libere espacio o que le toque el turno actual
		pthread_cond_wait(&cond_capacity, &mutex_capacity); //Debe siempre ir dentro de un while
	}

	printf("Client %2d VIP enters the disco.\n", id);

	++occupation; //Se aumenta la ocupacion de la discoteca
	printf("Actual occupation %d\n", occupation);

	++vips.actual; //Se aumenta en 1 en turno de la cola de vips

	pthread_mutex_unlock(&mutex_capacity); //Debloquea al mutex
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mutex_capacity);
	//SECCION CRITICA

	if(occupation> 0) --occupation;

	printf("Client %2d (%s) EXITS the disco\n", id, VIPSTR(isvip));
	printf("Acctual occupation: %d\n", occupation);

	//Se les dice a todos los hilos que estan esperando a que cambie la variable de condicion en sus respectivos
	// while que la variable ha cambiado. A partir de ahi se gestiona que entre un cliente vip o no.
	pthread_cond_broadcast(&cond_capacity);

	pthread_mutex_unlock(&mutex_capacity);
}

void *client(void *arg)
{
	int _id = ((struct client *)arg)->id;
	int _is_vip = ((struct client *)arg)->is_vip;

	if (_is_vip)
			enter_vip_client(_id);
	else enter_normal_client(_id);
	dance(_id, _is_vip);
	disco_exit(_id, _is_vip);

	pthread_exit(0); //Como el cliente ya ha acabado de hacer cosas, se acaba el hilo
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: %s <input_file.txt>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *file;

	if((file=fopen(argv[1], "r"))==NULL){
		printf("File %s could not be opened.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int numClients;
	//Lee la primera linea del fichero, parsea de string o char a int y lo guarda en numClients
	fscanf(file, "%d", &numClients);

	//Array de hilos
	pthread_t *threads = (pthread_t *)malloc(numClients*sizeof(pthread_t*));

	pthread_mutex_init(&mutex_capacity, NULL);
	pthread_cond_init(&cond_capacity, NULL);

	//Clientes
	struct client *clients = (struct client*)malloc(numClients * sizeof(struct client));

	//Se crea la lista de clientes
	for (int i = 0; i < numClients; ++i){
		int clientType;
		fscanf(file, "%d", &clientType);
		clients[i].id = i;
		clients[i].is_vip = clientType;

		//Se crea un hilo para cada cliente. A partir de aqui todo se gestiona desde la funcion client
		pthread_create(&threads[i], NULL, client, &clients[i]);
	}

	//Se van destruyendo los hilos cuando estos acaben
	for (int i = 0; i < numClients; ++i){
		pthread_join(threads[i], NULL);
	}

	//Se destruyen todos los mutex, variables de condicion, arrays, etc
	pthread_mutex_destroy(&mutex_capacity);
	pthread_cond_destroy(&cond_capacity);

	free(clients);
	free(threads);
	fclose(file);

	return 0;
}
