#include "db.h"

void dbInit(sqlite3* db){
	char* errMsg = 0;
	const char* sql = "CREATE TABLE IF NOT EXIST names (id INTEGER PRIMARY KEY AUTOINCREMENT, nome TEXT UNIQUE);";
	
	if(sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK){
		fprintf(stderr, "SQL Error: %s\n", err_msg);
		sqlite3_free(err_msg);
	}
}

void addName(sqlite3* db, const char* name){
	
}

void retrieveNames(sqlite3* db, int namesNumber){
	
}