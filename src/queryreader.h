#ifndef _QUERYREADER_H_
#define _QUERYREADER_H_

#include "assignment3.h"

	// Constants
	#define PARSED_PREFIX 2

	// Prototyping
	int qr_readquery(char *filepath);
	int qr_check_empty(char *line);
	void qr_trim_whitespace(char *line);

	int qr_parseline(char *line);
	void qr_add_prefix(char *line, int index);
	void qr_parse_select(char *line);
	void qr_parse_where(char *line);
	void qr_parse_query(char *line, int num_var);

	void qr_addmemto_triple(int entry_num);
	void qr_parse_period(char *line);
	void qr_parse_comma(char *line);
	void qr_parse_semicolon(char *line);
	void qr_parse_object(char *line);
	void qr_parse_predicate(char *line);
	void qr_parse_subject(char *line);

	// Test prototypes
	void qr_print_prefixes(void);
	int qr_printfile(char *filepath);

	// Globals
	extern a3_Prefix *prefixes;
	extern char *var_names;
	extern a3_Triple *triples;
	extern char **var_names;
#endif