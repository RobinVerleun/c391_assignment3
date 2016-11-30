#ifndef _QUERYREADER_H_
#define _QUERYREADER_H_

#include "assignment3.h"

	// Constants
	#define PARSED_PREFIX 2

	// Struct Definition
	typedef struct Variables {
		char *name;
		char usages[VARSIZE * 5];
		int used;
		int in_select;
	} ReturnVariable;

	// Prototyping
	
	// Get and prep lines
	int qr_readquery(char *filepath);
	int qr_check_empty(char *line);
	void qr_trim_whitespace(char *line);

	// Parse the different line conditions
	int qr_parseline(char *line);
	void qr_add_prefix(char *line, int index);
	void qr_add_varname(char *line);
	void qr_parse_select(char *line);
	void qr_parse_where(char *line);
	void qr_parse_query(char *line, int num_var);

	// Parse the different sections of a line
	void qr_addmemto_queries(int entry_num);
	void qr_parse_period(char *line);
	void qr_parse_comma(char *line);
	void qr_parse_semicolon(char *line);
	void qr_parse_object(char *line);
	void qr_parse_predicate(char *line);
	void qr_parse_subject(char *line);

	// Build a SQLite Query
	void qr_build_query(char **);

	// Test prototypes
	void qr_print_prefixes(void);
	void qr_print_varnames(void);
	int qr_printfile(char *filepath);

	// Globals
	extern a3_Prefix *prefixes;
	extern Query *queries;
	extern ReturnVariable *var_names;

#endif