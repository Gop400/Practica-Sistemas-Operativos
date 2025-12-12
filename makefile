CFLAGS = -Wall -g
CC = gcc

p3: main.o list.o aux.o p3.o
	$(CC) $(CFLAGS) -o p3 main.o list.o aux.o p3.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

p3.o: p3.c
	$(CC) $(CFLAGS) -c p3.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

aux.o: aux.c
	$(CC) $(CFLAGS) -c aux.c

valgrind: p3
	valgrind --leak-check=full ./p3

clean:
	rm -f p3 main.o list.o aux.o p3.o
