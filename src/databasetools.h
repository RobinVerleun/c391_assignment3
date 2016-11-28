#ifndef _DATABASETOOLS_H_
#define _DATABASETOOLS_H_

#include "sqlite3.h"
#include "assignment3.h"

	// Prototyping
	int dbt_open();
	int dbt_create_table();
	int dbt_insert(a3_Triple *triple);
	int dbt_close(); //TODO: Implement

	// Testing
	int dbt_simple_select();

	// Globals
	extern char *database_name;

#endif