#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h> //Para getopt

#define TAM 128

/** RESPUESTAS A LAS PREGUNTAS DEL EJERCICIO
 * 
 1. Al usar la opción -x del programa, el comando indicado como argumento se pasa encerrado entre comillas dobles en 
 el caso de que este, a su vez, acepte argumentos, como por ejemplo ls -l. ¿Qué ocurre si el argumento de -x no se 
 pasa entrecomillado? ¿Funciona correctamente el lanzamiento del programa ls -l si se encierra entre comillas simples 
 en lugar de dobles? Nota: Para ver las diferencias prueba a ejecutar el siguiente comando: echo $HOME
 
 Si se ejecuta -x ls -l comprende -x ls, pero -l lo parsea como una opción al igual que lo son -x y -s. Entonces da error
 de uso.
 Se obtiene lo siguiente por consola:

 Makefile  run_commands  run_commands.c  run_commands.o  test1  test2 --> Hace bien -x ls, pero no -l
./run_commands: invalid option -- 'l'   --> No encuentra la opción -l en las pasadas a getopt()
Usage: ./run_commands [-x <command>] [-s <filename>]

Con entrecomillado simple puede parecer que funciona, pero en el caso de usar un comando echo, no se va a ejecutar
correctamente:
 
./run_commands -x "echo $HOME"
/home/hlocal
./run_commands -x 'echo $HOME'
$HOME



2. ¿Es posible utilizar execlp() en lugar de execvp() para ejecutar el comando pasado como parámetro a la función 
launch_command() ? En caso afirmativo, indica las posibles limitaciones derivadas del uso de execlp() en este contexto.

Sí. Execlp puede ejecutar una serie de comandos cuyo número es conocido en tiempo de compilación. Sin embargo,
execvp es mejor usarla si no sabemos el número de comandos que se van a recibir para la ejecución de cada uno.
A execlp debemos pasarle una lista de argumentos conocidos desde el principio. A execvp se le pasa un vector que se
crea un tiempo de ejecución a medida que se van leyendo comandos. Se podrían usar ambos, pero en caso de querer cambiar
a execlp deberíamos modificar el codigo para obtener dichos argumentos antes de pasarlos al proceso hijo.



3. ¿Qué ocurre al ejecutar el comando "echo hola > a.txt" con ./run_commands -x ? ¿y con el comando 
"cat run_commands.c | grep int" ? En caso de que los comandos no se ejecuten correctamente indica el motivo.

./run_commands -x "echo hola > a.txt"
hola > a.txt
/run_commands -x "cat run_commands | grep int"
--> Se imprime todo el archivo run_commands ejecutable (con sus errores de expresión de caracteres ASCII) y tras eso
vemos que aparecen los errores:
cat: grep: No existe el archivo o el directorio
cat: int: No existe el archivo o el directorio

En niguno de los ejemplos se ejecutan correctamente los comandos: la opcion -x parsea solo un comando y no una 
sucesión de ellos. Tampoco parsea símbolos como > o |. y launch_command solo ejecuta el primero que se le pasa.
 
 */


pid_t launch_command(char** argv){
    
    pid_t pid; //id del proceso, es decir, identificador
    pid = fork(); //Devuelve 0 si se trata del proceso hijo y el pid del hijo si se trata del padre

    if(pid==0){ //Proceso hijo
        //Funcion terminativa: si funciona no se ejecuta nada que haya despues
        int ret = execvp(argv[0], argv);
        //Tratamiento de errores
        if(ret <0){
            perror("execvp error\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(pid >0 ){ //Proceso padre
        waitpid(pid, NULL, 0); //Es un waitpid clasico donde el padre espera a que el hijo termine
    }
    else {
        printf("Fork error\n");
        exit(EXIT_FAILURE);
    }

    return pid;
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}


int main(int argc, char *argv[]) {
    char **cmd_argv;
    char buffer[TAM]; //Tiene que ser un array de chars

    if (argc <1) {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    int opt, parsed_argc;
    //man 3 getopt, x espera argumento asi que se ponen dos puntos. b no espera nada asi que no hace falta
    while ((opt = getopt(argc, argv, "x:s:b")) != -1) {
        switch (opt)
        {
        case 'x':
        //No se puede pasar optind a parse_command porque varía el valor de optind y el bucle no acaba nunca
            cmd_argv=parse_command(optarg, &parsed_argc);
            launch_command(cmd_argv);
            break;

        case 's':
            FILE* f = fopen(optarg, "r");

            while((fgets( buffer,sizeof(char)*TAM,f))!=NULL){
                cmd_argv=parse_command(buffer, &parsed_argc); //Ídem en este caso de parse_command
                launch_command(cmd_argv);
            }

            break;

        case 'b': //Parte opcional. A implementar
            break;

         default: /* '?' */
                   fprintf(stderr, "Usage: %s [-x <command>] [-s <filename>]\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
        }
        
    }

    //Print parsed arguments
    // printf("argc: %d\n", cmd_argc);
    // for (i = 0; cmd_argv[i] != NULL; i++) {
    //     printf("argv[%d]: %s\n", i, cmd_argv[i]);
    //     free(cmd_argv[i]);  // Free individual argument
    // }

    free(cmd_argv);  // Free the cmd_argv array

    return EXIT_SUCCESS;
}