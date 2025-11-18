

EXECUTABLE = p1
CFLAGS = -Wall -g
OBJETOS = main.o p1.o list.o p0.o aux.o

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJETOS)
	$(CC) -o $(EXECUTABLE) $(OBJETOS)

# Reglas para compilar los .c
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

p1.o: p1.c
	$(CC) $(CFLAGS) -c p1.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

p0.o: p0.c
	$(CC) $(CFLAGS) -c p0.c

aux.o: aux.c
	$(CC) $(CFLAGS) -c aux.c


clean:
	rm -f $(EXECUTABLE) $(OBJETOS)

.PHONY: FORCE
$(OBJETOS): FORCE
