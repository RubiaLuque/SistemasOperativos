CC = gcc
CFLAGS = -Wall -g
LIBS = -lm #Añade librerias matematicas que no entran en include math.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#Instruccion que hace por defecto al hacer make
all : auxiliar.o init.o auxiliar.h
	gcc $(CFLAGS) -o prueba auxiliar.o init.o $(LIBS)

#Crea palabras reservadas por si estas existen como nombres de archivos para que no se confunda
.PHONY: clean

#Instrucciones adicionales que se pueden añadir
#En este caso hacer make clear borra todos los archivos .o y el de prueba
clean:
	-rm *.o prueba
