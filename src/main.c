#include "main.h"

int main(int argc, char **argv) {

	if( fr_readfile(argv[1]) ) {
		return 1;
	}

	//fr_print_prefixes();

	//fr_printfile(argv[1]);
	return 0;
}