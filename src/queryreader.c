#include "queryreader.h"

ReturnVariable *var_names;
int var_names_count = 0;

a3_Prefix *prefixes;
int current_prefix_count = 0;
int prefix_array_size = 0;

Query *queries;
char sub[URL_MAX], prd[URL_MAX], obj[URL_MAX];
int var_loc = 0;
char query_var[VARSIZE];

int query_array_size = 0;
int current_query = 0;

char line_delimiter = '.';
int return_all = 0;


/////////////////////////////////////////////////////////////////////////////////
// Read a file line by line. Each line is passed into a parser. Triples are then
// created and stored in an array of triples so that a sqlite query
// can be constructed and used to query our relational database
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
// Checks the line of the query given to it for only blank spaces to see if 
// it is empty
/////////////////////////////////////////////////////////////////////////////////
int qr_check_empty(char *line) {
	while(line[0] == ' ' || line[0] == '\t' || line[0] == '\n') { 
		if ('\n' == line[0]) {
			return SUCCESS;
		} else {
			line ++;
		}
	}
	return FAILURE;
}

/////////////////////////////////////////////////////////////////////////////////
// Trim whitespace characters off the beginning and end of a line read 
// from file
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
			line[i] = '\0';
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Takes in a line. Handles the purpose of the line in our relational format.
// ie. stores prefix in array to later create the uri, parse select statement for 
// return variables of our constructed query, parse where clause to then parse
// sub, prd, obj of the clause 
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
	} else if (!strstr(copy, "}")&&!strstr(copy, "WHERE")&&!strstr(copy, "where")&&!strstr(copy, "{")) {
		qr_parse_where(copy);
	}

	free(copy);
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a prefix to the external prefix array. If the prefix array is 
// too small, double the size of the array. Stores the shorthand and URI.
///////////////////////////////////////////////////////////////////////////////
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
// Dynamically allocates our query array.
/////////////////////////////////////////////////////////////////////////////////
void qr_addmemto_queries(int entry_num) {

	// Check the array. If query count is larger, double size and record. 
	if(entry_num > query_array_size) {
		queries = realloc(queries, 2 * entry_num * sizeof(Query));
		query_array_size = 2 * entry_num;
	} 	
}

/////////////////////////////////////////////////////////////////////////////////
// If a line contains the SELECT keyword, it is sent here to be parsed. Counts
// the number of distinc variables and then begins storing information about 
// them. If SELECT * is the option, a flag is triggered stating all variables
// will be used.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_select(char * line) {
	
	char *count_line = strdup(line);

	// Count the number of variables
	strtok(count_line, " ");
	char *var = strtok(NULL, " ");

	if(strchr(var, '*')){
		return_all = 1;
		return;
	}

	while(var != NULL) {
		if(strchr(var, '?')) {
			var_names_count++;
		}
		var = strtok(NULL, " ");
	}

	var_names = malloc(var_names_count * sizeof(ReturnVariable));
	int i = 0;

	// Store the names
	strtok(line, " ");
	var = strtok(NULL, " ");
	while(var != NULL) {
		if(strchr(var, '?')) {
			var_names[i].name = strdup(var);
			var_names[i].used = 0;
			var_names[i].in_select = 1;
			memset(var_names[i].usages, '\0', VARSIZE * 2);
			i++;
		}
		var = strtok(NULL, " ");
	}
}
	

/////////////////////////////////////////////////////////////////////////////////
// Parses the where clause(s) of the given query by first capturing the line
// delimiter and based on the previous delimiter (defualted to '.') parse the
// sub, prd, obj necessary. Since sub, prd, obj are global we only rewrite the
// URI when necessary.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_where(char * line) {

	
	// Dynamically reallocate memory for queries for every where clause line
	qr_addmemto_queries(current_query + 1);

	// Line delimiter tells us how to deal with the sub, prd, obj triples
	switch(line_delimiter) {
		
		case '.' :
			line_delimiter = line[strlen(line)-1];
			qr_parse_period(line);
			break;
		case ',' :
			line_delimiter = line[strlen(line)-1];
			qr_parse_comma(line);
			break;
		case ';' :
			line_delimiter = line[strlen(line)-1];
			qr_parse_semicolon(line);
			break;
		default :
		 fprintf(stderr, "File is improperly formatted.");
	}

	strcpy(queries[current_query].triple.sub, sub);
	strcpy(queries[current_query].triple.prd, prd);
	strcpy(queries[current_query].triple.obj, obj);
	strcpy(queries[current_query].var_name, query_var);
	queries[current_query].var_loc = var_loc;

	/*
	printf("START HERE:%d: Sub:%s\n Prd:%s\n Obj:%s\n Varloc:%d\n VarName:%s\n", 
		current_query, queries[current_query].triple.sub, 
		queries[current_query].triple.prd, queries[current_query].triple.obj, 
		queries[current_query].var_loc, queries[current_query].var_name);
	
	*/
	current_query++;	
}

/////////////////////////////////////////////////////////////////////////////////
// Take in a variable found during parsing. If the variable hasn't already been
// stored from an earlier line, creates space for the variable and stores
// relevant information. Affected by the SELECT * global.
/////////////////////////////////////////////////////////////////////////////////
void qr_add_varname(char *line) {
	int i;
	int include = 1;
	for(i = 0; i < var_names_count; i++) {
		if(strcmp(line, var_names[i].name) == 0) {
			include = 0;
		}
	}
	if(include) {
		var_names_count++;
		var_names = realloc(var_names, var_names_count * sizeof(ReturnVariable));
		var_names[var_names_count - 1].name = strdup(line);
		
		if(return_all) {
			var_names[var_names_count - 1].in_select = 1;
			var_names[var_names_count - 1].used = 0;
		} else {
			var_names[var_names_count - 1].in_select = 0;
			var_names[var_names_count - 1].used = 1;
		}
		memset(var_names[var_names_count - 1].usages, '\0', VARSIZE * 2);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that all values read belong into the triple 
// object. Only called if line_delimiter == '.' from the previous line.
// Saves the last character before the line is altered.
///////////////////////////////////////////////////////////////////////////////
void qr_parse_period(char *line) {
	char *subj, *pred, *objt;

	subj = strtok(line, "\t ");
	line++;
	pred = strtok(NULL, "\t ");
	line++;
	objt = strtok(NULL, "\n\0");
	
	if(strchr(line, ' ')) {
		objt[strlen(objt) - 2] = '\0';
	} else {
		objt[strlen(objt) - 1] = '\0';
	}

	qr_parse_subject(subj);
	qr_parse_predicate(pred);
	qr_parse_object(objt);
}

///////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that only the object field of the triple
// will be changed. Only called if line_delimiter == ',' from the previous line.
// Saves the last character before the line is altered.
///////////////////////////////////////////////////////////////////////////////
void qr_parse_comma(char *line) {

	char *objt = strtok(line, "\n\0");

	if(strchr(line, ' ')) {
		objt[strlen(objt) - 2] = '\0';
	} else {
		objt[strlen(objt) - 1] = '\0';
	}

	while (' ' == objt[0] || '\t' == objt[0]) {
		objt ++;
	}
	qr_parse_object(objt);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses the line, knowing that only the object, and predicate field of the triple
// will be changed. Only called if line_delimiter == ';' from the previous line.
// Saves the last character before the line is altered.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_semicolon(char *line) {
	char *pred, *objt;

	pred = strtok(line, "\t");
	line++;
	objt = strtok(NULL, "\n\0");
	
	if(strchr(line, ' ')) {
		objt[strlen(objt) - 2] = '\0';
	} else {
		objt[strlen(objt) - 1] = '\0';
	}

	while (' ' == objt[0] || '\t' == objt[0]) {
		objt ++;
	}
	qr_parse_predicate(pred);
	qr_parse_object(objt);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our subject parameter given as a string containing only the subject field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field. If it is a variable it also stores that 
// specially.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_subject(char *line) {
	char full_URI[URL_MAX];
	char *store;
	int i;

	if(line[0] == '?') {
		strcpy(full_URI, line);
		strcpy(query_var, line);
		var_loc = 1;
		qr_add_varname(line);

	} else if(line[0] == '<') {
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

	strcpy(sub, full_URI);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our predicate parameter given as a string containing only the predicate field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field. If it is a variable it also stores that 
// specially.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_predicate(char *line) {
	char full_URI[URL_MAX];
	char *store;
	int i;

	if(line[0] == '?') {
		strcpy(full_URI, line);
		strcpy(query_var, line);
		qr_add_varname(line);
		var_loc = 2;

	} else	if(line[0] == '<') {
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
	strcpy(prd, full_URI);
}

/////////////////////////////////////////////////////////////////////////////////
// Parses our object parameter given as a string containing only the object field
// Checks for all possible value types it could be and parses accordingly. Then 
// creates the full URI for the field. If it is a variable it also stores that 
// specially.
/////////////////////////////////////////////////////////////////////////////////
void qr_parse_object(char *line) {
	
	char obj_URI[URL_MAX];
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

	if ('?' == line[0]) {
		strcpy(obj_URI, line);
		strcpy(query_var, line);
		qr_add_varname(line);
		var_loc = 3;

	} else if ('<' == curChar) {  //Regular full address
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
			strcpy(obj_URI, prd);
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
		strcpy(obj_URI, prd);
		strcat(obj_URI, store);
	}
	// Store the obj in our triple struct
	strcpy(obj, obj_URI);
}

/////////////////////////////////////////////////////////////////////////////////
// Builds the query based of all parsed data. Goes through all stored query
// structs and begins piecing together the SELECT, FROM, and WHERE SQLite 
// statements. JOIN is a special buffer used to link multiple references
// to the same variables.
/////////////////////////////////////////////////////////////////////////////////
void qr_build_query(char **finalResult) {

	// Initialize buffers
	char SELECT[200];
	char FROM[200];
	char WHERE[1000];
	char JOIN[1000];
	strcpy(SELECT, "SELECT ");
	strcpy(FROM, "FROM ");
	strcpy(WHERE, "WHERE ");
	strcpy(JOIN, "");

	// Iterate through the parsed query statements
	int i, j;
	for(i = 0; i < current_query; i++) {

		char buf[URL_MAX];
		// Build the Select statement

		// Iterate through ALL the variables found
		for(j = 0; j < var_names_count; j++) {
	
			// Check if the var name was retrieved from the select statement
			// and if it hasn't been used yet
			if(var_names[j].in_select && !var_names[j].used){

				// Ensure that this var_name matches the one for the query we're examing
				// to ensure that the data for the whole loop is consistent
				if(0 == strcmp(queries[i].var_name, var_names[j].name)) {
				
					// Set the variable to used so it will not be reincluded in SELECT
					var_names[j].used = 1;

					if(1 == queries[i].var_loc) {
						// Create the query string, append to SELECT, and store usage
						// in the ReturnVariable struct
						sprintf(buf, "t%d.sub,", i);
						strcat(SELECT, buf);
						strcpy(var_names[j].usages, buf);

					}
					else if(2 == queries[i].var_loc) {
						sprintf(buf, "t%d.prd,", i);
						strcat(SELECT, buf);
						strcpy(var_names[j].usages, buf);
					}
					else {
						sprintf(buf, "t%d.obj,", i);
						strcat(SELECT, buf);
						strcpy(var_names[j].usages, buf);
					}
				}
			}
		}
		
		// Build the From statement
		sprintf(buf, "relational_rdf t%d,", i);
		strcat(FROM, buf);


		// Build the Where statement
		if(1 == queries[i].var_loc) {
			for(j = 0; j < var_names_count; j++) {
				if(strcmp(queries[i].var_name, var_names[j].name) == 0) {
					sprintf(buf, "t%d.sub,", i);
					if(!strstr(var_names[j].usages, buf)) {
						strcat(var_names[j].usages, buf);
					}
				}
			}
			sprintf(buf, "t%d.prd = \"%s\" AND t%d.obj = \"%s\" AND ", 
				i, queries[i].triple.prd, i, queries[i].triple.obj);
			strcat(WHERE, buf);
		}
		else if(2 == queries[i].var_loc) {
			for(j = 0; j < var_names_count; j++) {
				if(strcmp(queries[i].var_name, var_names[j].name) == 0) {
					sprintf(buf, "t%d.prd,", i);
					if(!strstr(var_names[j].usages, buf)) {
						strcat(var_names[j].usages, buf);
					}
				}
			}
			sprintf(buf, "t%d.sub = \"%s\" AND t%d.obj = \"%s\" AND ", 
				i, queries[i].triple.sub, i, queries[i].triple.obj);
			strcat(WHERE, buf);
		}
		else {
			for(j = 0; j < var_names_count; j++) {
				if(strcmp(queries[i].var_name, var_names[j].name) == 0) {
					sprintf(buf, "t%d.obj,", i);
					if(!strstr(var_names[j].usages, buf)) {
						strcat(var_names[j].usages, buf);
					}
				}
			}
			sprintf(buf, "t%d.sub = \"%s\" AND t%d.prd = \"%s\" AND ", 
				i, queries[i].triple.sub, i, queries[i].triple.prd);
			strcat(WHERE, buf);
		}
	}

	// Make the final join
	for(i = 0; i < var_names_count; i++) {

		// Make a safe copy
		char *cp = strdup(var_names[i].usages);
		char buf[50];

		// Going through each variable found
		char *first_var = strtok(cp, ",");
		char *second_var = strtok(NULL, ",");
		while(second_var != NULL) {

			//Blank space if left to overwrite in case of AND
			sprintf(buf, "%s=%s     ", first_var, second_var);
			strcat(JOIN, buf);
			second_var = strtok(NULL, ",");
		}
		int j;
		for(j = 0; j < strlen(JOIN); j++) {
			if(JOIN[j] == ' ') {
				JOIN[j+1] = 'A';
				JOIN[j+2] = 'N';
				JOIN[j+3] = 'D';
				JOIN[j+4] = ' ';
				j = j+4;
			}
		}

	}	

	
	SELECT[strlen(SELECT) - 1] = ' ';
	FROM[strlen(FROM) - 1] = ' ';
	if(strlen(JOIN) > 1) {
		JOIN[strlen(JOIN) - 5] = ';';
		JOIN[strlen(JOIN) - 4] = '\0';
	} else {
		WHERE[strlen(WHERE) - 5] = '\0';
		JOIN[0] = ';';
		JOIN[1] = '\0';
	}

	*finalResult = malloc(strlen(SELECT) + strlen(FROM) + strlen(WHERE) + strlen(JOIN));
	strcpy(*finalResult, SELECT);
	strcat(*finalResult, FROM);
	strcat(*finalResult, WHERE);
	strcat(*finalResult, JOIN);
	//printf("%s\n", *finalResult);

	qr_print_output();

}

/////////////////////////////////////////////////////////////////////////////////
// Prints the output variables desired by the given query.
/////////////////////////////////////////////////////////////////////////////////
void qr_print_output() {

	int k;

	printf("%s\t\t\t", var_names[0].name);

	for (k = 1; k < var_names_count; k++)  {
		printf("||%s\t\t\t", var_names[k].name);
	}

	printf("\n");

}

/////////////////////////////////////////////////////////////////////////////////
// Helper function
/////////////////////////////////////////////////////////////////////////////////
void qr_print_prefixes() {
	int i;
	for(i = 0; i < current_prefix_count; i++) {
		printf("%d) %s : %s\n", i, prefixes[i].shorthand, prefixes[i].uri);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Helper function
/////////////////////////////////////////////////////////////////////////////////
void qr_print_varnames() {
	int i;
	for(i = 0; i < var_names_count; i++) {
		printf("%d) Var:%s usages:%s\n", i, var_names[i].name, var_names[i].usages);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Helper function
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