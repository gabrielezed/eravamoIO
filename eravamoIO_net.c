/*
 This part of the code is written by a Large Language Model.
 I didn't want to investigate microhttpd library for this project, as it is meant to be just a fun project to laugh with my friends.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include "lib/sqlite3.h"
#include "eravamoIO_ops.h"
#include "eravamoIO_types.h"
#include "eravamoIO_net.h"

struct MHD_Daemon* httpServerDaemon;
sqlite3* globalDbConnection;

enum MHD_Result requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr) {
    (void)cls;
    (void)version;

    if (strcmp(method, "POST") == 0 && *uploadDataSize != 0) {
        (void)uploadData;
        *uploadDataSize = 0;
        return MHD_YES;
    }

    if (*ptr == NULL) {
        *ptr = (void*)1;
        return MHD_YES;
    }
    *ptr = NULL;

    struct MHD_Response* httpResponse;
    enum MHD_Result requestResult;

    if (strcmp(method, "GET") == 0 && strcmp(url, "/meme") == 0) {
        const char* countParam = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "count");
        int namesNumber = 3;
        
        if (countParam != NULL) {
            namesNumber = atoi(countParam);
            if (namesNumber < 3) {
                namesNumber = 3;
            }
            if (namesNumber > 50) {
                namesNumber = 50;
            }
        }

        StringArray namesList = eravamoIO_retrieveNames(globalDbConnection, namesNumber);
        
        if (namesList.len < 3) {
            const char* errorMsg = "Not enough names";
            httpResponse = MHD_create_response_from_buffer(strlen(errorMsg), (void*)errorMsg, MHD_RESPMEM_PERSISTENT);
            requestResult = MHD_queue_response(connection, 400, httpResponse);
            MHD_destroy_response(httpResponse);
            cleanStringArray(&namesList);
            return requestResult;
        }

        size_t totalLength = strlen("Eravamo IO") + 2;
        for (size_t i = 0; i < namesList.len; i++) {
            totalLength += strlen(namesList.data[i]) + 2;
        }

        char* responseBuffer = malloc(totalLength);
        if (responseBuffer != NULL) {
            strcpy(responseBuffer, "Eravamo IO");
            for (size_t i = 0; i < namesList.len; i++) {
                strcat(responseBuffer, ", ");
                strcat(responseBuffer, namesList.data[i]);
            }
            strcat(responseBuffer, ".");

            httpResponse = MHD_create_response_from_buffer(strlen(responseBuffer), (void*)responseBuffer, MHD_RESPMEM_MUST_FREE);
            requestResult = MHD_queue_response(connection, 200, httpResponse);
            MHD_destroy_response(httpResponse);
        } else {
            const char* errorMsg = "Internal Server Error";
            httpResponse = MHD_create_response_from_buffer(strlen(errorMsg), (void*)errorMsg, MHD_RESPMEM_PERSISTENT);
            requestResult = MHD_queue_response(connection, 500, httpResponse);
            MHD_destroy_response(httpResponse);
        }

        cleanStringArray(&namesList);
        return requestResult;
    }

    if (strcmp(method, "POST") == 0 && strcmp(url, "/add") == 0) {
        const char* nameParam = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "name");
        
        if (nameParam != NULL) {
            char* nameCopy = strdup(nameParam);
            
            if (sanityCheck(nameCopy) != NULL) {
                eravamoIO_addName(globalDbConnection, nameCopy);
                const char* successMsg = "Name added successfully";
                httpResponse = MHD_create_response_from_buffer(strlen(successMsg), (void*)successMsg, MHD_RESPMEM_PERSISTENT);
                requestResult = MHD_queue_response(connection, 200, httpResponse);
            } else {
                const char* errorMsg = "Invalid name format";
                httpResponse = MHD_create_response_from_buffer(strlen(errorMsg), (void*)errorMsg, MHD_RESPMEM_PERSISTENT);
                requestResult = MHD_queue_response(connection, 400, httpResponse);
            }
            free(nameCopy);
        } else {
            const char* errorMsg = "Missing name parameter";
            httpResponse = MHD_create_response_from_buffer(strlen(errorMsg), (void*)errorMsg, MHD_RESPMEM_PERSISTENT);
            requestResult = MHD_queue_response(connection, 400, httpResponse);
        }
        
        MHD_destroy_response(httpResponse);
        return requestResult;
    }

    const char* notFoundMsg = "Endpoint not found";
    httpResponse = MHD_create_response_from_buffer(strlen(notFoundMsg), (void*)notFoundMsg, MHD_RESPMEM_PERSISTENT);
    requestResult = MHD_queue_response(connection, 404, httpResponse);
    MHD_destroy_response(httpResponse);
    
    return requestResult;
}

void startHttpServer(int port, sqlite3* dbConnection) {
    globalDbConnection = dbConnection;
    httpServerDaemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL, &requestHandler, NULL, MHD_OPTION_END);
}

void stopHttpServer() {
    if (httpServerDaemon != NULL) {
        MHD_stop_daemon(httpServerDaemon);
    }
}