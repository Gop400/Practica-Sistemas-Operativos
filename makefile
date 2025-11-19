

CFLAGS = -Wall -g
CC = gcc


p2: main.o p1.o list.o p0.o aux.o p2.o
	$(CC) $(CFLAGS) -o p2 main.o p1.o list.o p0.o aux.o p2.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

p1.o: p1.c
	$(CC) $(CFLAGS) -c p1.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

p0.o: p0.c
	$(CC) $(CFLAGS) -c p0.c

p2.o: p2.c
	$(CC) $(CFLAGS) -c p2.c

aux.o: aux.c
	$(CC) $(CFLAGS) -c aux.c

valgrind: p2
	valgrind --leak-check=full ./p2

clean:
	rm -f p2 main.o p1.o list.o p0.o aux.o p2.o
