#include "q8.h"

int main(int argc, char **argv) {

	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s <Database Name> <RDF Datapath>\n", argv[0]);
		return FAILURE;
	}

	database_name = malloc(strlen(argv[1]));
	strcpy(database_name, argv[1]);

	if( dbt_open() == FAILURE ) {
		return FAILURE;
	}
	if( dbt_create_table() == FAILURE ) {
		return FAILURE;
	}

	if( fr_readfile(argv[2]) == FAILURE ) {
		return FAILURE;
	}

	free(database_name);
	return 0;
}