#include "q9.h"

int main(int argc, char **argv) {

	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s <Database Name> <SPARQL Query Filepath>\n", argv[0]);
		return FAILURE;
	}

	database_name = malloc(strlen(argv[1]));
	strcpy(database_name, argv[1]);

	if( dbt_open() == FAILURE ) {
		return FAILURE;
	}

	if(qr_readquery(argv[2]) == FAILURE ) {
		return FAILURE;
	}

	char *sql_stmt;
	qr_build_query(&sql_stmt);
	printf("%s\n", sql_stmt);

	if(dbt_query(sql_stmt) == FAILURE ){
		return FAILURE;
	}

	free(queries);
	free(var_names);
	free(prefixes);
	return 0;
}