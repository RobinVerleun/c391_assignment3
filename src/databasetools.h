#ifndef _DATABASETOOLS_H_
#define _DATABASETOOLS_H_

#include "sqlite3.h"
#include "assignment3.h"

	// Constants
	#define DATABASE "./resources/test.db"
	// Prototyping
	int dbt_open();
	//int dbt_insert(a3_Triple *triple);

	// Testing
	int dbt_simple_select();

	// Globals
	

#endif