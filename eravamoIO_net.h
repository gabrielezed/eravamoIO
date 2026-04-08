#ifndef ERAVAMOIO_NET_H
#define ERAVAMOIO_NET_H

#include "lib/sqlite3.h"

void startHttpServer(int port, sqlite3* dbConnection);
void stopHttpServer();

#endif