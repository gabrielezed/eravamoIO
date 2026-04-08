#ifndef ERAVAMOIO_CLI_H
#define ERAVAMOIO_CLI_H

#include "lib/sqlite3.h"

void helpMessage();
void unrecognizeArgument();
int parseArgument(int argc, char** argv, sqlite3* dbConnection);

#endif