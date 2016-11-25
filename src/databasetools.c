#include "databasetools.h"

sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;

int dbt_open() {
	rc = sqlite3_open(DATABASE, &db);
    if( rc ) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
     	sqlite3_close(db);
		return 1;
  	}
  	return 0;
}

int dbt_simple_select() {
	char *sql_stmt = "SELECT * FROM test_table;";

	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if( rc != SQLITE_OK ) {
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	while(sqlite3_step(stmt) == SQLITE_ROW) {
		printf("%s\n", sqlite3_column_text(stmt, 0));
	}

	sqlite3_finalize(stmt);
	return 0;
}