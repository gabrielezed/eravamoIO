#include <stdio.h>
#include "lib/sqlite3.h"
#include "lib/eravamoIO_types.h"
#include "lib/eravamoIO_db.h"

void generateMeme(StringArray namesList){
	if(namesList.len < 3){
		fprintf(stderr, "Not enough names to generate a phrase!\n");
		return;
	}

	printf("Eravamo IO");
	for(size_t i = 0; i < namesList.len; i++){
		printf(", %s", namesList.data[i]);
	}
	printf(".\n");
}

void cleanStringArray(StringArray *a){
	for(size_t i = 0; i < a->len; i++){
		free(a->data[i]);
	}
	StringArray_free(a);
}

int main(int argc, char** argv){
	printf("Hello, Sassari!\n");

	sqlite3 *db;
	if(sqlite3_open("eravamo.db", &db) != SQLITE_OK){
		fprintf(stderr, "Error while trying to open database: %s\n", sqlite3_errmsg(db));
	}

	eravamoIO_dbInit(db);

	/*eravamoIO_addName(db, "Nicotina Nick");
	eravamoIO_addName(db, "Grimy Joe");
	eravamoIO_addName(db, "Megan Chill");
	eravamoIO_addName(db, "Crash");
	eravamoIO_addName(db, "Signor. Tupa");
	eravamoIO_addName(db, "Charlie Charleston");
	eravamoIO_addName(db, "i Motoroil");*/

	generateMeme(eravamoIO_retrieveNames(db, 3));

	
	return 0;
}
