#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eravamoIO_db.h"
#include "sqlite3.h"

void eravamoIO_dbInit(sqlite3* db){
	char* errMsg = 0;
	const char* sql = "CREATE TABLE IF NOT EXISTS names (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE);";
	
	if(sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK){
		fprintf(stderr, "SQL Error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
}

void eravamoIO_addName(sqlite3* db, const char* name){
	sqlite3_stmt *pS;
	const char *sqlQuery = "INSERT OR IGNORE INTO names (name) VALUES (?);";

	if(sqlite3_prepare_v2(db, sqlQuery, -1, &pS, 0) != SQLITE_OK){
		fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
	}

	if(sqlite3_bind_text(pS, 1, name, -1, SQLITE_TRANSIENT) != SQLITE_OK){
		fprintf(stderr, "Bind failed: %s\n", sqlite3_errmsg(db));
	}
	
	if(sqlite3_step(pS) != SQLITE_DONE){
		fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(pS);
}

StringArray eravamoIO_retrieveNames(sqlite3* db, int namesNumber){
	StringArray result;
	StringArray_init(&result);

	sqlite3_stmt *pS;
	const char* sqlQuery = "SELECT name FROM names ORDER BY RANDOM() LIMIT ?;";

	if(sqlite3_prepare_v2(db, sqlQuery, -1, &pS, 0) != SQLITE_OK) return result;

	if(sqlite3_bind_int(pS, 1, namesNumber) != SQLITE_OK){
		sqlite3_finalize(pS);
		return result;
	}

	while(sqlite3_step(pS) == SQLITE_ROW && result.len < (size_t)namesNumber){
		const char* retrievedName = (const char*)sqlite3_column_text(pS, 0);
		if(retrievedName != NULL){
			size_t nameLength = strlen(retrievedName);
			char* nameCopy = malloc(nameLength + 1);
			if(nameCopy != NULL){
				strcpy(nameCopy, retrievedName);
				StringArray_push(&result, nameCopy);
			}
		}
	}

	sqlite3_finalize(pS);
	return result;
}
