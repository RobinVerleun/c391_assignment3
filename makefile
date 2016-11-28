CC=gcc
CFLAGS=-g -Wall
CLFLAGS=-lpthread -ldl
SOURCES=q8.c q9.c
OBJECTS=$(SOURCES:.c=.o)
Q8=./src/q8.c ./src/filereader.c ./src/databasetools.c ./src/sqlite3.c
Q9=./src/q9.c ./src/queryreader.c ./src/databasetools.c ./src/sqlite3.c
DATABASE=*.db
EXECUTABLES=$(OBJECTS:.o=)

all: $(OBJECTS)

q8.o:
	@$(CC) $(CFLAGS) $(Q8) -o q8 $(CLFLAGS)

q9.o:
	@$(CC) $(CFLAGS) $(Q9) -o q9 $(CLFLAGS)

clean:
	@rm -f $(EXECUTABLES) $(DATABASE)