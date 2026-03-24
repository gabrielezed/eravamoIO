#ifndef DB_H
#define DB_H

#include "sqlite3.h"

void dbInit(sqlite3* db);
void addName(sqlite3* db, const char* name);
void retrieveNames(sqlite3* db, int namesNumber);


#endif