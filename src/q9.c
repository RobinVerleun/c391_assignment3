#include "q9.h"

int main(int argc, char **argv) {

	//qr_printfile("./query.txt");
	qr_readquery("./query.txt");
	qr_build_query();
	//printf("%s\n", prefixes[0].uri);
	//qr_print_prefixes();

	free(queries);
	free(var_names);
	free(prefixes);
	return 0;
}