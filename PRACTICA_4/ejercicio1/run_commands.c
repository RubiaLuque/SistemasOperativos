#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h> //Para getopt

#define TAM 128

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