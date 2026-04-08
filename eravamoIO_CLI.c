#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "eravamoIO_CLI.h"
#include "eravamoIO_ops.h"

void helpMessage(){
	printf("----------------------------eravamoIO-------------------------\n");
	printf("| -h --> Displays this help message                          |\n");
	printf("| -i 'name'  --> Adds a name to the db                       |\n");
	printf("| -o --> Generates a phrase                                   |\n");
	printf("|-------------------------------------------------------------\n");
	
}

void unrecognizeArgument(){
	printf("Unrecognized flag. Use eravamoIO -h to view usage. \n");
}

int parseArgument(int argc, char** argv, sqlite3* dbConnection){
	int opt;
	while((opt = getopt(argc, argv, "i:oh")) != -1){
		switch(opt){
			case 'i':
				char* validatedName = sanityCheck(optarg);
                if (validatedName != NULL) {
                    eravamoIO_addName(dbConnection, validatedName);
                }
				break;
			case 'o':{
				StringArray a = eravamoIO_retrieveNames(dbConnection, 5);
				generateMeme(a);
				cleanStringArray(&a);
				break;
			}
			case 'h':
				helpMessage();
				return 0;
				break;
			case '?':
				unrecognizeArgument();
				return 0;
				break;
		}
		
	}
	
	return 1;
}