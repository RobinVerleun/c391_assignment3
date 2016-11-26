CC=gcc
CFLAGS=-g -Wall
CLFLAGS=-lpthread -ldl
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
MAIN=./src/main.c ./src/filereader.c ./src/databasetools.c ./src/sqlite3.c
EXECUTABLE=main
DATABASE=*.db

all: $(OBJECTS)

main.o:
	@$(CC) $(CFLAGS) $(MAIN) -o $(EXECUTABLE) $(CLFLAGS)

clean:
	@rm $(EXECUTABLE) $(DATABASE)