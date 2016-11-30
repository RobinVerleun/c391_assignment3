#include "q9.h"

int main(int argc, char **argv) {

	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s <Database Name> <SPARQL Query Filepath>\n", argv[0]);
		return FAILURE;
	}

	database_name = malloc(strlen(argv[1]));
	strcpy(database_name, argv[1]);

	dbt_open();

	qr_readquery(argv[2]);

	char *sql_stmt;
	qr_build_query(&sql_stmt);

	dbt_query(sql_stmt);
	//printf("%s\n", prefixes[0].uri);
	//qr_print_prefixes();

	free(queries);
	free(var_names);
	free(prefixes);
	return 0;
}