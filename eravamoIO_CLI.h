#ifndef ERAVAMOIO_CLI
#define ERAVAMOIO_CLI

#include "lib/sqlite3.h"

void helpMessage();
void unrecognizeArgument();
int parseArgument(int argc, char** argv, sqlite3* dbConnection);

#endif