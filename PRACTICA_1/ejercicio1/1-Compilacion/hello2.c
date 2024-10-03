#include <stdio.h>

#define N 5

#define min(x,y) ( (x<y)?x:y )
int a = 7;
int b = 9;

int main() {

 char* cad = "Hello world";
 int i;

 for (i=0;i<N;i++) {
   printf("%s \t a= %d b= %d\n",cad,a,b);
   a++;
   a = min(a,b);
 }
 return 0;
}

//si se hace 
/*
gcc -o hello2 hello2.c <-- para compilar el archivo
./hello2  <-- ejecutar
gcc -o hello2 hello2.c --save-temps  <-- para obtener todos los archivos intermedios (es mejor que usar -E)

    ¿Qué ha ocurrido con la “llamada a min()” en hello2.i?
Se ha sustituido por su definicion. Lo mismo ha pasado con N, que es 5

    ¿Qué efecto ha tenido la directiva #include <stdio.h>?
Que se incluyan multitud de ficheros extra al comienzo que se encuentran en /usr/include/ y /usr/lib/

*/