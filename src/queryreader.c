#include "queryreader.h"

a3_Prefix *prefixes;
int current_prefix_count = 0;
int prefix_array_size = 0;

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
int qr_readquery(char *filepath) {
	FILE *ptr;
	char *buf;
	//a3_Triple triple;
	
	// Setup 
	prefixes = NULL;
	buf = malloc(BUFFSIZE);
	ptr = fopen(filepath, "r");
	if( !ptr ) {
		fprintf(stderr, "Error opening file <%s>.\n", filepath);
		return FAILURE;
	}

	// Main functionality 
	int rc;
	while( fgets(buf, BUFFSIZE, ptr) != NULL ) {
		if( qr_check_empty(buf) == FAILURE) {
			qr_trim_whitespace(buf);
			qr_parseline(buf);
		}
	}

	// Teardown 
	fclose(ptr);
	free(buf);
	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
int qr_check_empty(char *line) {
	while(line[0] == ' ' || line[0] == '\t' || line[0] == '\n') { 
		if ('\n' == line[0]) {
			return SUCCESS:
		} else {
			line ++;
		}
	}
	return FAILURE;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void qr_trim_whitespace(char *line) {
	//TODO: Trim whitespace
	while (' ' == line[0] || '\t' == line[0]) {
		line ++;
	}
	// Replace newline with null character 
	int i; 
	for(i = 0; line[i]; i++) {
		if (line[i] == '\n') {
			line[i] == '\0';
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
int qr_parseline(char *line) {

	char *copy = malloc(strlen(line));
	strcpy(copy, line);					// Create a safe to handle copy
	
	// Check for prefix
	if(strstr(copy, "PREFIX") || strstr(copy, "prefix")) {
		qr_add_prefix(copy, current_prefix_count + 1);
		current_prefix_count++;
		free(copy);
		return PARSED_PREFIX;
	}
	//Check for SUBJECT
	else if(strstr(copy, "SELECT") || strstr(copy, "select")) {
		qr_parse_select(copy);
	}

	free(copy);
	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void qr_add_prefix(char *line, int entry_num) {

	// Check the array. If prefix_count is larger, double size and record. 
	if(entry_num > prefix_array_size) {
		prefixes = realloc(prefixes, 2 * entry_num * sizeof(a3_Prefix));
		prefix_array_size = 2 * entry_num;
	}

	line += 7;			// Skips the 'prefix ' portion of the line 

	char *cur_short = strtok(line, ":");
	strtok(NULL, "<");
	char *cur_uri = strtok(NULL, ">");

	strcpy(prefixes[current_prefix_count].shorthand, cur_short);
	strcpy(prefixes[current_prefix_count].uri, cur_uri); 	
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_select(char * line) {
	
	strtok(line, " ");
	char *var = strtok(NULL, " ");
	
	while(var != NULL) {
		if(strchr(var, '?')){
			printf("%s\n", var);
		}
		var = strtok(NULL, " ");
		//TODO:This currently isolates the ? items in the if statement. Find a way
		//to store them so they can be returned later, and matched properly
		//to the sub-prd-obj format. There may be more in the query.
	}
	
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_where(char * line) {

}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void qr_print_prefixes() {
	int i;
	for(i = 0; i < current_prefix_count; i++) {
		printf("%d) %s : %s\n", i, prefixes[i].shorthand, prefixes[i].uri);
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
int qr_printfile(char *filepath) {
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