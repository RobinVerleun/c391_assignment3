#include "filereader.h"

a3_Prefix *prefixes;
int current_prefix_count = 0;
int prefix_array_size = 0;
char line_delimiter;

///////////////////////////////////////////////////////////////////////////////
// Read a file line by line. Each line is passed into a parser, along with 
// a structure to hold the triple. It is then passed to SQLite and inserted
// into a table.
///////////////////////////////////////////////////////////////////////////////
int fr_readfile(char *filepath) {
	FILE *ptr;
	char *buf;
	a3_Triple triple;
	
	/* Setup */
	prefixes = NULL;
	buf = malloc(BUFFSIZE);
	ptr = fopen(filepath, "r");
	if( !ptr ) {
		fprintf(stderr, "Error opening file <%s>.\n", filepath);
		return 1;
	}

	/* Main functionality */
	while( fgets(buf, BUFFSIZE, ptr) != NULL ) {
		fr_parseline(buf, &triple);
	}



	/* Teardown */
	fclose(ptr);
	free(buf);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Take in a line. Store the last element of the line.
// Check if the line is a prefix - if so, pass the line to 
// fr_add_prefix. Otherwise, assigned values to the triple, and remember
// the final character for future parsing.
///////////////////////////////////////////////////////////////////////////////
int fr_parseline(char *line, a3_Triple *triple) {

	
	
	/* Check if the line is a prefix - handles specially */ 
	if( '@' == line[0] ) {

		line_delimiter = line[strlen(line)-2];
		fr_add_prefix(line, current_prefix_count + 1);
		current_prefix_count++;

	} else {

		switch(line_delimiter) {
			case '.' :
				//printf("Period.\n");
				fr_parse_period(line, triple);
				break;
			case ',' :
				//printf("Comma.\n");
				break;
			case ';' :
				//printf("Semicolon.\n");
				break;
			default :
			 fprintf(stderr, "File is improperly formatted.");
			 return 1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a prefix to the external prefix array. If the prefix array is 
// too small, double the size of the array. Stores the shorthand and URI.
///////////////////////////////////////////////////////////////////////////////
void fr_add_prefix(char *line, int entry_num) {

	/* Check the array. If prefix_count is larger, double size and record. */
	if(entry_num > prefix_array_size) {
		prefixes = realloc(prefixes, 2 * entry_num * sizeof(a3_Prefix));
		prefix_array_size = 2 * entry_num;
	}

	line += 8;			/* Skips the '@prefix ' portion of the line */

	char *cur_short = strtok(line, ":");
	strtok(NULL, "<");
	char *cur_uri = strtok(NULL, ">");

	strcpy(prefixes[current_prefix_count].shorthand, cur_short);
	strcpy(prefixes[current_prefix_count].uri, cur_uri); 	
}

///////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that all values read belong into the triple 
// object. Only called if line_delimiter == '.' from the previous line.
// Saves the last character before the line is altered.
///////////////////////////////////////////////////////////////////////////////
void fr_parse_period(char *line, a3_Triple *triple) {
	
	char full_URI[URL_MAX];		/* Used for the final results */
	char *store;			/* Used during parsing for temp storage */
	int i;

	line_delimiter = line[strlen(line)-2];

	///////////////////////////////////////////////////////////////////////
	
	/* Get the Subject information by prefix shorthand */
	store = NULL;
	store = strtok(line, ":");
	for(i = 0; i < prefix_array_size; i++) {
		if(strcmp(prefixes[i].shorthand, store) == 0) {
			strcpy(full_URI, prefixes[i].uri);
			break;
		}
	}
	/* Append the resource to the prefix and store it in triple */
	line++;
	strcat(full_URI, strtok(NULL, "\t"));
	strcpy(triple->sub, full_URI);

	///////////////////////////////////////////////////////////////////////

	/* Get the Predicate information by prefix shorthand */
	store = NULL;
	line++;
	store = strtok(NULL, ":");
	for(i = 0; i < prefix_array_size; i++) {
		if(strcmp(prefixes[i].shorthand, store) == 0) {
			strcpy(full_URI, prefixes[i].uri);
			break;
		}
	}
	/* Append the resource to the prefix and store it in triple */
	line++;
	strcat(full_URI, strtok(NULL, "\t"));
	strcpy(triple->prd, full_URI);

	///////////////////////////////////////////////////////////////////////

	/* Get the Object information by prefix shorthand */
	store = NULL;
	line++;
	store = strtok(NULL, ":");
	
	/* Special check for literals */
	char check = store[strlen(store)-2];
	if(check == '.' || check == ',' || check == ';') {
		char lit[50];
		strncpy(lit, store, strlen(store)-3);
		strcpy(triple->obj, lit);
	} else {
		for(i = 0; i < prefix_array_size; i++) {
			if(strcmp(prefixes[i].shorthand, store) == 0) {
				strcpy(full_URI, prefixes[i].uri);
				break;
			}
		}
		/* Append the resource to the prefix and store it in triple */
		line++;
		strcat(full_URI, strtok(NULL, " "));
		strcpy(triple->obj, full_URI);
		
	}
	printf("%s \t %s \t %s\n", triple->sub, triple->prd, triple->obj);
}

int fr_printfile(char *filepath) {
	FILE *ptr;
	char *buf;

	/* Setup */
	buf = malloc(BUFFSIZE);
	ptr = fopen(filepath, "r");
	if( !ptr ) {
		fprintf(stderr, "Error opening file <%s>.\n", filepath);
		return 1;
	}

	/* Main functionality */
	while( fgets(buf, BUFFSIZE, ptr) != NULL ) {
		printf("%s\n", buf);
	}

	/* Teardown */
	fclose(ptr);
	free(buf);
	return 0;
}

void fr_print_prefixes() {
	int i;
	for(i = 0; i < current_prefix_count; i++) {
		printf("%d) %s : %s\n", i, prefixes[i].shorthand, prefixes[i].uri);
	}
}