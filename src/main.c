#include "main.h"

int main(int argc, char **argv) {

	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s <Database Name> <RDF Datapath>\n", argv[0]);
		return 1;
	}

	database_name = malloc(strlen(argv[1]));
	strcpy(database_name, argv[1]);

	if( dbt_open() ) {
		return 1;
	}
	if( dbt_create_table() ) {
		return 1;
	}

	if( fr_readfile(argv[2]) ) {
		return 1;
	}


	//fr_print_prefixes();

	//fr_printfile(argv[1]);

	free(database_name);
	return 0;
}