EXECUTABLE=p0

CFLAGS= -Wall -g


# Regla para construir el ejecutable


# Regla para limpiar los archivos generados
shell: main.c
	gcc $(CFLAGS) -o $(EXECUTABLE)  list.c main.c p0.c aux.c

clean:

	rm -f $(EXECUTABLE)
