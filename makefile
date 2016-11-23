CC=gcc
CFLAGS=-g -Wall
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
MAIN=./src/main.c ./src/filereader.c
EXECUTABLE=main

all: $(OBJECTS)

main.o:
	@$(CC) $(CFLAGS) $(MAIN) -o $(EXECUTABLE)

clean:
	@rm $(EXECUTABLE)