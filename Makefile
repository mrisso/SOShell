### Makefile ###

#Compilador
CC = gcc

#Opções
CFLAGS = -Wall

#Fontes
FONTES = $(wildcard *.c)

#Objetos
OBJ = $(FONTES:.c=.o)

#Executável
EXEC = csh

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $^

clean:
	rm -rf *.o $(EXEC)
