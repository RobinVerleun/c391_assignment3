#include "databasetools.h"

char *database_name;

sqlite3 *db;
sqlite3_stmt *stmt;
int rc;

int dbt_open() {
	rc = sqlite3_open(database_name, &db);
    if( rc ) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
     	sqlite3_close(db);
		return FAILURE;
  	}
  	return SUCCESS;
}

int dbt_create_table() {
	char *sql_stmt = 	"CREATE TABLE IF NOT EXISTS relational_rdf"
						"(sub varchar(200), "
						"prd varchar(200), "
						"obj varchar(200));";

	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Preparation failed (dbt_create_table): %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FAILURE;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Step failed (dbt_create_table): %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FAILURE;
	}

	sqlite3_reset(stmt);
	return 0;
}

int dbt_insert(a3_Triple *triple) {
	char * sql_stmt = 	"INSERT INTO relational_rdf(sub, prd, obj) "
						"VALUES (?, ?, ?);";

	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Preparation failed (dbt_insert): %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FAILURE;
	}

	rc = sqlite3_bind_text(stmt, 1, triple->sub, -1, SQLITE_STATIC); 
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Subject binding failed (dbt_insert): %s\n", sqlite3_errmsg(db));
		return FAILURE;
	}
	rc = sqlite3_bind_text(stmt, 2, triple->prd, -1, SQLITE_STATIC); 
	if(rc != SQLITE_OK) {	
		fprintf(stderr, "Predicate binding failed (dbt_insert): %s\n", sqlite3_errmsg(db));
		return FAILURE;
	}
	rc = sqlite3_bind_text(stmt, 3, triple->obj, -1, SQLITE_STATIC);
	if(rc != SQLITE_OK) {	
		fprintf(stderr, "Object binding failed (dbt_insert): %s\n", sqlite3_errmsg(db));
		return FAILURE;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Step failed (dbt_insert): %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FAILURE;
	}
	

	sqlite3_clear_bindings(stmt);
	
	sqlite3_reset(stmt);
	return SUCCESS;
}

int dbt_query(char *sql_stmt) {
	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Step failed (dbt_query): %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return FAILURE;
	}

	while(sqlite3_step(stmt) == SQLITE_ROW) {
		printf("%s\n", sqlite3_column_text(stmt, 0));
	}

	return SUCCESS;
}

/*
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
*/