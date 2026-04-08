#ifndef ERAVAMOIO_OPS_H
#define ERAVAMOIO_OPS_H

#include "lib/sqlite3.h"
#include "eravamoIO_types.h"


//Database operations
void eravamoIO_dbInit(sqlite3* db);
void eravamoIO_addName(sqlite3* db, const char* name);
StringArray eravamoIO_retrieveNames(sqlite3* db, int namesNumber);

//Core operations
char* sanityCheck(char* s);
void generateMeme(StringArray namesList);

#endif