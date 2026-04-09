#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eravamoIO_CLI.h"
#include "eravamoIO_ops.h"
#include "eravamoIO_types.h"
#include "eravamoIO_net.h"

void helpMessage(){
	printf("----------------------------eravamoIO-------------------------\n");
	printf("| -d         --> Run as HTTP Daemon                          |\n");
	printf("| -p <port>  --> Specify daemon port                         |\n");
	printf("| -i <name>  --> Adds a name to the db                       |\n");
	printf("| -o <count> --> Generates a phrase with <count> names       |\n");
	printf("| -h         --> Displays this help message                  |\n");
	printf("|-------------------------------------------------------------\n");
	
}

void unrecognizeArgument(){
	printf("Unrecognized flag. Use eravamoIO -h to view usage. \n");
}

int parseArgument(int argc, char** argv, sqlite3* dbConnection){
	bool isDaemon = false;
	int serverPort = 6666;
	char* nameToAdd = NULL;
	int genCount = 0;

	int opt;
	while((opt = getopt(argc, argv, "dp:i:o:h")) != -1){
		switch(opt){
			case 'd':
				isDaemon = true;
				break;
			case 'p':
				serverPort = atoi(optarg);
				break;
			case 'i':
				nameToAdd = optarg;
				break;
			case 'o':
				genCount = atoi(optarg);
				break;
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
	
	if(nameToAdd != NULL){
		if(sanityCheck(nameToAdd) != NULL){
			eravamoIO_addName(dbConnection, nameToAdd);
			printf("Name added successfully!\n");
		}
	}

	if(genCount > 0){
		if(genCount < 3) genCount = 3;
		StringArray a = eravamoIO_retrieveNames(dbConnection, genCount);
		generateMeme(a);
		cleanStringArray(&a);
	}
	else if(genCount < 0) {
		fprintf(stderr, "Please provide a <count> major than 3.");
		return 1;
	}

	if(isDaemon){
		printf("Starting HTTP daemon on port %d\n", serverPort);
		startHttpServer(serverPort, dbConnection);
		printf("Press ENTER to stop the server.\n");
		getchar();
		stopHttpServer();
		printf("Server stopped.\n");
	}

	return 0;
}
