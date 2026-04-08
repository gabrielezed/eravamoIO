#include <stdio.h>
#include <stdlib.h>

#include "lib/sqlite3.h"
#include "eravamoIO_ops.h"
#include "eravamoIO_net.h"


int main(int argc, char** argv){
	printf("Hello, Sassari!\n");
	
	sqlite3* dbConnection;
	int serverPort = 6666;
	
	if(sqlite3_open("eravamo.db", &dbConnection) != SQLITE_OK){
		fprintf(stderr, "Error while trying to open database: %s\n", sqlite3_errmsg(dbConnection));
		return 1;
	}	
	
	sqlite3_busy_timeout(dbConnection, 5000);
	sqlite3_exec(dbConnection, "PRAGMA journal_mode=WAL", 0, 0, 0);
	
	eravamoIO_dbInit(dbConnection);
	
	startHttpServer(serverPort, dbConnection);
	getchar();
	
	stopHttpServer();	
	
	//parseArgument(argc, argv, dbConnection);

	/*eravamoIO_addName(db, "Nicotina Nick");
	eravamoIO_addName(db, "Grimy Joe");
	eravamoIO_addName(db, "Megan Chill");
	eravamoIO_addName(db, "Crash");
	eravamoIO_addName(db, "Signor. Tupa");
	eravamoIO_addName(db, "Charlie Charleston");
	eravamoIO_addName(db, "i Motoroil");*/

	sqlite3_close(dbConnection);
	
	return 0;
}
