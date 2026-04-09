#include <stdio.h>

#include "eravamoIO_CLI.h"
#include "lib/sqlite3.h"
#include "eravamoIO_ops.h"


int main(int argc, char** argv){
	printf("Hello, Sassari!\n");
	
	sqlite3* dbConnection;
	
	if(sqlite3_open("eravamo.db", &dbConnection) != SQLITE_OK){
		fprintf(stderr, "Error while trying to open database: %s\n", sqlite3_errmsg(dbConnection));
		return 1;
	}	
	
	sqlite3_busy_timeout(dbConnection, 5000);
	sqlite3_exec(dbConnection, "PRAGMA journal_mode=WAL", 0, 0, 0);
	
	eravamoIO_dbInit(dbConnection);
	
	parseArgument(argc, argv, dbConnection);

	sqlite3_close(dbConnection);
	
	return 0;
}
