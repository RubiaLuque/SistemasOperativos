#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    while(1){ //Bucle infinito: while true

        sleep(1); //Duerme por 1 segundo
        printf("%s\n", argv[1]); //Imprime lo que se pase como argumento
    }
}

/*
En consola se puede hacer ./a.out HOLA & ./a.out ADIOS se van ejecutando los dos al mismo tiempo, pero no tienen por qué estar 
sincronizados. Si hacemos ctrl-c solamente pararemos el segundo programa y el primero seguirá en el background. Habría que hacer fg 1
para traerlo al foreground. Si quiero enviar ambos al background se añadiria otro & despues del segundo programa.

Nota: se usa fg 1 o fg 2 porque son los numeros asociados al proceso de cada programa en este caso.

Nota 2: cada consola tiene sus propios recursos y no se podrían parar estos procesos desde otra consola.

*/