CC=gcc
CFLAGS=-g -Wall
CLFLAGS=-lpthread -ldl
SOURCES=q8.c
OBJECTS=$(SOURCES:.c=.o)
Q8=./src/q8.c ./src/filereader.c ./src/databasetools.c ./src/sqlite3.c
EXECUTABLE=q8
DATABASE=*.db

all: $(OBJECTS)

q8.o:
	@$(CC) $(CFLAGS) $(Q8) -o $(EXECUTABLE) $(CLFLAGS)

clean:
	@rm $(EXECUTABLE) $(DATABASE)