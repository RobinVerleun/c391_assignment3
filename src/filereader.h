#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include "assignment3.h"
#include "databasetools.h"

	// Constants
	#define PARSED_PREFIX 2
	// Prototyping 
	int fr_readfile(char *filepath);		
	int fr_parseline(char *line, a3_Triple *triple);
	void fr_add_prefix(char *line, int);
	void fr_parse_period(char *line, a3_Triple *triple);
	void fr_parse_comma(char *line, a3_Triple *triple);
	void fr_parse_semicolon(char *line, a3_Triple *triple);

	void fr_parse_object(char *line, a3_Triple *triple);
	void fr_parse_predicate(char *line, a3_Triple *triple);
	void fr_parse_subject(char *line, a3_Triple *triple);
	
	// Globals
	extern a3_Prefix *prefixes;

	// Testing Prototypes 
	int fr_printfile(char *filepath);
	void fr_print_prefixes(void);

#endif