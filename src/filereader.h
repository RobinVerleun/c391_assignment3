#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include "assignment3.h"

	/* Prototyping */
	int fr_readfile(char *filepath);		/* Read a file line by line into memory. Return 0 on succes */
	int fr_parseline(char *line, a3_Triple *triple); 		/* Takes in a line that's read and parses it as needed */
	void fr_add_prefix(char *line, int);
	void fr_parse_period(char *line, a3_Triple *triple);
	void fr_parse_comma(char *line, a3_Triple *triple);
	void fr_parse_semicolon(char *line, a3_Triple *triple);

	void fr_parse_object(char *line, a3_Triple *triple);
	void fr_parse_predicate(char *line, a3_Triple *triple);
	void fr_parse_subject(char *line, a3_Triple *triple);
	/* Testing Prototypes */
	int fr_printfile(char *filepath);		/* Print a file as it's read line by line. Return 0 on succes */
	void fr_print_prefixes(void);
	/* Globals */


#endif




	/*


	parse_period
		parse subject
		parse predicate
		parse parse object

	parse_comma
		parse object

	parse semicolon
		parse predicate
		parse object
	*/