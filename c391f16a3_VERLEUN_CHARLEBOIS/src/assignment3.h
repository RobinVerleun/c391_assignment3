#ifndef _ASSIGNMENT3_H_
#define _ASSIGNMENT3_H_

// Libraries used across all files 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

	// Constants 
	#define BUFFSIZE 500
	#define URL_MAX 200
	#define VARSIZE 30
	#define SUCCESS 0
	#define FAILURE 1

	// Struct definitions 
	typedef struct Triples {
		char sub[URL_MAX];
		char prd[URL_MAX];
		char obj[URL_MAX];
	} a3_Triple;

	typedef struct Queries {
		a3_Triple triple;
		int var_loc;
		char var_name[VARSIZE];
		
	} Query;

	typedef struct Prefixes {
		char shorthand[VARSIZE];
		char uri[URL_MAX];
	} a3_Prefix;

	// Globals 
	


#endif