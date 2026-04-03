#ifndef ERAVAMOIO_DB_H
#define ERAVAMOIO_DB_H

#include "sqlite3.h"
#include "eravamoIO_types.h"

void eravamoIO_dbInit(sqlite3* db);
void eravamoIO_addName(sqlite3* db, const char* name);
StringArray eravamoIO_retrieveNames(sqlite3* db, int namesNumber);


#endif
