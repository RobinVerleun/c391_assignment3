#include "filereader.h"

a3_Prefix *prefixes;
int current_prefix_count = 0;
int prefix_array_size = 0;
char line_delimiter = '.';

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
		return FAILURE;
	}

	/* Main functionality */
	int rc;
	while( fgets(buf, BUFFSIZE, ptr) != NULL ) {
		rc = fr_parseline(buf, &triple);
		if( rc != PARSED_PREFIX ) {
			rc = dbt_insert(&triple);
			if( rc == FAILURE ) {
				return FAILURE;
			}
		}
	}

	/* Teardown */
	fclose(ptr);
	free(buf);
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Take in a line. Store the last element of the line.
// Check if the line is a prefix - if so, pass the line to 
// fr_add_prefix. Otherwise, assigned values to the triple, and remember
// the final character for future parsing.
///////////////////////////////////////////////////////////////////////////////
int fr_parseline(char *line, a3_Triple *triple) {

	// Remove leading whitespace 
	while(line[0] == ' ' || line[0] == '\t') { line ++; }

	// Check if the line is a prefix - handles specially  
	if( '@' == line[0] ) {
		line_delimiter = line[strlen(line)-2];
		fr_add_prefix(line, current_prefix_count + 1);
		current_prefix_count++;
		return PARSED_PREFIX;

	} else {

		switch(line_delimiter) {
			
			case '.' :
				line_delimiter = line[strlen(line)-2];
				fr_parse_period(line, triple);
				//printf("%s | %s | %s\n", triple->sub, triple->prd, triple->obj);
				break;

			case ',' :
				line_delimiter = line[strlen(line)-2];
				fr_parse_comma(line, triple);
				//printf("%s | %s | %s\n", triple->sub, triple->prd, triple->obj);
				break;

			case ';' :
				line_delimiter = line[strlen(line)-2];
				fr_parse_semicolon(line, triple);
				//printf("%s | %s | %s\n", triple->sub, triple->prd, triple->obj);
				break;

			default :
			 fprintf(stderr, "File is improperly formatted.");
			 return FAILURE;
		}
	}
	return SUCCESS;
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
	char *subj, *pred, *obj;

	subj = strtok(line, "\t");
	line++;
	pred = strtok(NULL, "\t");
	line++;
	obj = strtok(NULL, "\n");
	obj[strlen(obj) - 2] = '\0';

	fr_parse_subject(subj, triple);
	fr_parse_predicate(pred, triple);
	fr_parse_object(obj, triple);
}

///////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that only the object field of the triple
// will be changed. Only called if line_delimiter == ',' from the previous line.
// Saves the last character before the line is altered.
///////////////////////////////////////////////////////////////////////////////
void fr_parse_comma(char *line, a3_Triple *triple) {
	char *obj = strtok(line, "\n");
	obj[strlen(obj) -2] = '\0';
	fr_parse_object(obj, triple);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that only the object, and predicate field of the triple
// will be changed. Only called if line_delimiter == ';' from the previous line.
// Saves the last character before the line is altered.
/////////////////////////////////////////////////////////////////////////////////
void fr_parse_semicolon(char *line, a3_Triple *triple) {
	char *pred, *obj;

	pred = strtok(line, "\t");
	line++;
	obj = strtok(NULL, "\n");
	obj[strlen(obj) -2] = '\0';

	fr_parse_predicate(pred, triple);
	fr_parse_object(obj, triple);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our subject parameter given as a string containing only the subject field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field.
/////////////////////////////////////////////////////////////////////////////////
void fr_parse_subject(char *line, a3_Triple *triple) {
	char full_URI[URL_MAX];
	char *store;
	int i;

	if(line[0] == '<') {
		line ++;
		store = strtok(line, ">");
		strcpy(full_URI, store);
	} else {

		store = strtok(line, ":");
		for(i = 0; i < prefix_array_size; i++) {
			if(strcmp(prefixes[i].shorthand, store) == 0) {
				strcpy(full_URI, prefixes[i].uri);
				break;
			}
		}	
		line++;
		strcat(full_URI, strtok(NULL, "\t"));
	}
	strcpy(triple->sub, full_URI);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our predicate parameter given as a string containing only the predicate field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field.
/////////////////////////////////////////////////////////////////////////////////
void fr_parse_predicate(char *line, a3_Triple *triple) {
	char full_URI[URL_MAX];
	char *store;
	int i;

	if(line[0] == '<') {
		line ++;
		store = strtok(line, ">");
		strcpy(full_URI, store);
	} else {

		store = strtok(line, ":");
		for(i = 0; i < prefix_array_size; i++) {
			if(strcmp(prefixes[i].shorthand, store) == 0) {
				strcpy(full_URI, prefixes[i].uri);
				break;
			}
		}	
		line++;
		strcat(full_URI, strtok(NULL, "\t"));

	}
	strcpy(triple->prd, full_URI);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our object parameter given as a string containing only the object field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field.
/////////////////////////////////////////////////////////////////////////////////
void fr_parse_object(char *line, a3_Triple *triple) {
	
	char obj_URI[URL_MAX];
	char *pred_URI = triple->prd;
	char curChar = line[0];
	char *store;
	int i;

	//Search for ^^ -- dealing with prefix call
	char *objpref;
	objpref = strchr(line, '^');
	//Search for @ -- dealing with a literal
	char *langlit;
	langlit = strchr(line, '@');
	//Search for : -- for cases where the line doesnt start with "
	char *objpage;
	objpage = strchr(line, ':');

	if ('<' == curChar) {  //Regular full address
		line ++;
		store = strtok(line, ">");
		strcpy(obj_URI, store);

	} else if ('"' == curChar) { // Deal with all three cases of obj's starting with "
		// deals with ^^
		if(objpref) {

			line = strtok(line, "^^");
			// Save this value to append later
			char *lineValue = line;

			strcpy(obj_URI, lineValue);
			strcat(obj_URI, "^^");

			line = strtok(NULL, "");
			line ++;

			store = strtok(line, ":");
			for(i = 0; i < prefix_array_size; i++) {
				if(strcmp(prefixes[i].shorthand, store) == 0) {
					strcat(obj_URI, prefixes[i].uri);
					break;
				}
			}

			// Append the resource to the prefix and store it in triple
			strcat(obj_URI, strtok(NULL, "\t"));
		
		// deals with @
		} else if(langlit) {
			store = strtok(line, "@");
			strcpy(obj_URI, store);

		// deals with string literal
		} else {
			line ++;
			line = strtok(line, "\"");
			store = line;
			strcpy(obj_URI, pred_URI);
			strcat(obj_URI, "#");
			strcat(obj_URI, store);
		}

	} else if (objpage) {
		store = strtok(line, ":");
		for(i = 0; i < prefix_array_size; i++) {
			if(strcmp(prefixes[i].shorthand, store) == 0) {
				strcpy(obj_URI, prefixes[i].uri);
				break;
			}
		}
		// Append the resource to the prefix and store it in triple 
		line++;
		strcat(obj_URI, strtok(NULL, ""));

	} else { // deals with literals
		store = line;
		strcpy(obj_URI, pred_URI);
		strcat(obj_URI, "#");
		strcat(obj_URI, store);
	}

	// Store the obj in our triple struct
	strcpy(triple->obj, obj_URI);

}

/////////////////////////////////////////////////////////////////////////////////
// Helper function
/////////////////////////////////////////////////////////////////////////////////
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
		printf("%s", buf);
	}

	/* Teardown */
	fclose(ptr);
	free(buf);
	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
// Helper functionality
/////////////////////////////////////////////////////////////////////////////////
void fr_print_prefixes() {
	int i;
	for(i = 0; i < current_prefix_count; i++) {
		printf("%d) %s : %s\n", i, prefixes[i].shorthand, prefixes[i].uri);
	}
}