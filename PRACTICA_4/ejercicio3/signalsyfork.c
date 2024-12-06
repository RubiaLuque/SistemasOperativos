#define _XOPEN_SOURCE 500 //Para uso de señales, se combina con la inclusion de signal.h
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //Para el uso de señales
#include <string.h> //Para el uso de strdup
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/wait.h> //Para la funcion wait

/*programa que temporiza la ejecución de un proceso hijo */

//Las señales se usan como medio de comunicacion asíncrona entre procesos

pid_t child_pid;

//Qué va a ocurrir cuando se reciba la señal de temporizador agotado
//Los parametros que tiene esta funcion dependen de qué opcion se haya elegido como handler. En este caso, al
// elegir sa_sigaction y tener el flag SA_SIGINFO, debe tener los parametros especificados a continuacion:
//(tipo de señal, informacion sobre el origen de la señal, informacion sobre el contexto de la señal al producirse
// la interrupcion)
void sigAlarmHandler(int sigType, siginfo_t* info, void* context){
	if(child_pid>0){
		kill(child_pid, SIGKILL);
	}
}

void sigIntHandler(int sigType){
	//printf("Program does not close\n");
}

//Se reciben como argumento el comando a ejecutar en el proceso hijo con sus argumentos seguidos si los hubiera
int main(int argc, char **argv)
{
	if(argc < 2){
		printf("Missing arguments.\n");
		exit(EXIT_FAILURE);
	}

	char* command_name = strdup(argv[1]);
	//Guarda los agumentos de command_name en caso de que se le hayan pasado
	char* arguments[argc-1]; //No es de tamaño argc-2 porque necesita que el ultimo parametro sea NULL

	if(argc > 2){ //Si se han pasado argumentos del comando se obtienen
		for(int i = 2; i<argc-2; ++i){
			//strdup nos permite obtener una copia sin necesidad de saber la longitud de la palabra como strcopy
			arguments[i] = strdup(argv[i]);
		}

		arguments[argc-2] = NULL; //Ultimo espacio a NULL
	}

	int status;

	child_pid = fork();

	if(child_pid == 0){ //Proceso hijo
		//Se usa execvp porque:
		// v --> se le pasan los argumentos en un vector (arguments en este caso)
		// p --> ejecuta un comando propio de linux que se encuentran en /bin por defecto
		execvp(command_name, arguments);

		perror("execvp");
		exit(EXIT_FAILURE);

	}else{ //Proceso padre
		//El padre es el que setea y maneja la alarma

		//CONFIGURACION DE LA ACCION A REALIZAR AL RECIBIR LA SEÑAL DESEADA

		//struct sigaction:
		// - handler --> funcion a ejecutar cuando se reciba la señal
				//Dos posibilidades que no se dan a la vez:
					// sa_handler --> la func handler solo recibe el primer argumento
					// sa_sigaction --> debe tener el flag SA_SIGINFO y la func handler recibe 3 parametros
			// - mask --> señales que se van a bloquear mientras se ejecuta handler
			// - flags --> comportamiento especial para la señal

		struct sigaction actionAlarm;
		actionAlarm.sa_sigaction = sigAlarmHandler; //Funcion que se debe ejecutar
		sigemptyset(&actionAlarm.sa_mask); //Se setea la mascara por defecto
		actionAlarm.sa_flags = SA_SIGINFO | SA_RESTART; //Flags que se eligen
		//SA_SIGINFO sirve para que devuelva mas informacion sobre la señal y SA_RESTART se usa para que
		// el SO no dé error al interrumpir un comando. De manera que vuelve a hacer la llamada al sistema de
		// nuevo para evitar un error por interrupción.


		//SETEO DE LA FUNCION SIGACTION
		//Esta funcion setea la accion dada por actionAlarm para SIGALRM, en este caso
		//Es decir, dada una señal, realiza la accion que se le pase como segundo parametro
		//El tercer parametro es para guaardar o no qué se hacía antes de llamar a la accion
		if(sigaction(SIGALRM, &actionAlarm, NULL) == -1){
			perror("sigaction");
			exit(EXIT_FAILURE);
		}

		/*
		- Si el proceso hijo tarda mas de 5 segundos, se envia la señal de alarma, que al tratarla se llama al
		handler y se envía SIGKILL para matar el proceso hijo, tal y como se pide.
		- Si tarda menos de 5 segundos, esta señal nunca se llega a enviar y el proceso se acaba de manera natural.
		*/

		//CONFIGURACIÓN DEL ENVIO DE LA SEÑAL SIGALRM TRAS 5 SEGS
		alarm(5);

		struct sigaction actionInt;
		//actionInt.sa_handler = sigIntHandler;
		actionInt.sa_handler = SIG_IGN;
		sigemptyset(&actionInt.sa_mask);
		actionInt.sa_flags = SA_RESTART;

		//SIGINT se suele producir cuando se hace Ctrl+c y se acaba el proceso, pero se puede sobreescribir para
		// que realice la accion que queramos. El programa que estaba siendo ejecutado por el proceso hijo
		// no ignora esta señal y sí se cierra abruptamente

		/*
		if(sigaction(SIGINT, &actionInt, NULL)==-1){
			perror("sigaction SIGINT\n");
			exit(EXIT_FAILURE);
		} */

		//Una vez configurado el manejo y recepcion de señales, el padre espera a que acabe el proceso hijo
		while(child_pid !=wait(&status)); //En la variable status recibimos el motivo de cierre del proceso

		//----aqui ya ha acbado el proceso hijo----
		//se cancela cualquier alarma pendiente de acabar:
		alarm(0);

		//Se comprueba el motivo de salida del proceso hijo:
		if(WIFEXITED(status)){ //--> salida normal por exit sin error o return desde main
			printf("Child process %d ended normally with value %d.\n", child_pid, WEXITSTATUS(status));
			//WEXITSTATUS contiene la informacion de salida del proceso hijo
		}
		else if(WIFSIGNALED(status)){ //--> Salida por una señal
			printf("Child process %d was terminated by a signal %d.\n", child_pid, WTERMSIG(status));
			//WTERMSIG devuelve el numero correspondiente a la señal que ha hecho acabar el proceso hijo
		}
	}


	for(int i = 0; i<argc-1; ++i){
		free(arguments[i]);
	}


	return 0;
}
