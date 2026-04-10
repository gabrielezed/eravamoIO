/*
 * This part of the code is written by a Large Language Model.
 * I didn't want to investigate microhttpd library for this project, as it is
 * meant to be just a fun project to laugh with my friends.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <microhttpd.h>

#include "lib/sqlite3.h"
#include "eravamoIO_ops.h"
#include "eravamoIO_types.h"
#include "eravamoIO_net.h"

#define MAX_PAYLOAD_SIZE 128
#define THREAD_POOL_CAPACITY 16

struct MHD_Daemon *serverDaemon = NULL;
sqlite3 *databaseConnection = NULL;
pthread_rwlock_t databaseRwLock = PTHREAD_RWLOCK_INITIALIZER;

struct ConnectionState {
	struct MHD_PostProcessor *postProcessor;
	char *postedName;
};

enum MHD_Result postIterator(void* connectionInfo, enum MHD_ValueKind kind,
                             const char *key, const char *filename,
                             const char *contentType,
                             const char *transferEncoding, const char *data,
                             uint64_t off, size_t size) {

	struct ConnectionState* state = (struct ConnectionState*)connectionInfo;

	if(strcmp(key, "name") == 0 && size > 0) {
		size_t currentLength = 0;

		if(state->postedName != NULL) {
			currentLength = strlen(state->postedName);
		}

		if(currentLength + size > MAX_PAYLOAD_SIZE) {
			return MHD_NO;
		}

		if(state->postedName == NULL) {
			state->postedName = malloc(size + 1);
			if(state->postedName != NULL) {
				memcpy(state->postedName, data, size);
				state->postedName[size] = '\0';
			}
		} else {
			char *newName = realloc(state->postedName, currentLength + size + 1);
			if(newName != NULL) {
				state->postedName = newName;
				memcpy(state->postedName + currentLength, data, size);
				state->postedName[currentLength + size] = '\0';
			}
		}
	}

	return MHD_YES;
}

void requestCompleted(void* cls, struct MHD_Connection* connection,
                      void **connectionContext, enum MHD_RequestTerminationCode toe) {
	struct ConnectionState* state = (struct ConnectionState*)*connectionContext;

	if(state != NULL) {
		if(state->postProcessor != NULL) {
			MHD_destroy_post_processor(state->postProcessor);
		}
		if(state->postedName != NULL) {
			free(state->postedName);
		}
		free(state);
		*connectionContext = NULL;
	}
}

enum MHD_Result sendResponse(struct MHD_Connection* connection, int statusCode, const char* content) {
	struct MHD_Response* response = MHD_create_response_from_buffer(strlen(content), (void*)content, MHD_RESPMEM_MUST_COPY);

	if(response == NULL) {
		return MHD_NO;
	}

	MHD_add_response_header(response, "Content-Type", "text/plain; charset=utf-8");
	MHD_add_response_header(response, "X-Content-Type-Options", "nosniff");

	enum MHD_Result result = MHD_queue_response(connection, statusCode, response);
	MHD_destroy_response(response);

	return result;
}

enum MHD_Result handleGetMeme(struct MHD_Connection* connection) {
	const char *countParameter = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "count");
	int numberOfNames = 3;

	if(countParameter != NULL) {
		numberOfNames = atoi(countParameter);
		if(numberOfNames < 3) {
			numberOfNames = 3;
		} else
			if(numberOfNames > 50) {
				numberOfNames = 50;
			}
	}

	pthread_rwlock_rdlock(&databaseRwLock);
	StringArray retrievedNames = eravamoIO_retrieveNames(databaseConnection, numberOfNames);
	pthread_rwlock_unlock(&databaseRwLock);

	if(retrievedNames.len < 3) {
		cleanStringArray(&retrievedNames);
		return sendResponse(connection, 400, "Not enough names");
	}

	size_t bufferSize = strlen("Eravamo IO") + 2;
	for(size_t i = 0; i < retrievedNames.len; i++) {
		bufferSize += strlen(retrievedNames.data[i]) + 2;
	}

	char *responseContent = malloc(bufferSize);
	if(responseContent == NULL) {
		cleanStringArray(&retrievedNames);
		return sendResponse(connection, 500, "Internal Server Error");
	}

	strcpy(responseContent, "Eravamo IO");
	for(size_t i = 0; i < retrievedNames.len; i++) {
		strcat(responseContent, ", ");
		strcat(responseContent, retrievedNames.data[i]);
	}
	strcat(responseContent, ".");

	enum MHD_Result result = sendResponse(connection, 200, responseContent);
	free(responseContent);
	cleanStringArray(&retrievedNames);

	return result;
}

enum MHD_Result handlePostAdd(struct MHD_Connection* connection, struct ConnectionState* state) {
	if(state->postedName == NULL) {
		return sendResponse(connection, 400, "Missing name parameter");
	}

	size_t nameLength = strlen(state->postedName);
	if(nameLength == 0 || nameLength > MAX_PAYLOAD_SIZE) {
		return sendResponse(connection, 400, "Invalid name length");
	}

	char *sanitizedName = sanityCheck(state->postedName);
	if(sanitizedName == NULL) {
		return sendResponse(connection, 400, "Invalid name format");
	}

	pthread_rwlock_wrlock(&databaseRwLock);
	eravamoIO_addName(databaseConnection, sanitizedName);
	pthread_rwlock_unlock(&databaseRwLock);

	return sendResponse(connection, 200, "Name added successfully");
}

enum MHD_Result mainRequestHandler(void* cls, struct MHD_Connection* connection,
                                   const char *url, const char *method,
                                   const char *version, const char *uploadData,
                                   size_t *uploadDataSize,
                                   void **connectionContext) {

	if(*connectionContext == NULL) {
		struct ConnectionState* state = malloc(sizeof(struct ConnectionState));
		if(state == NULL) {
			return MHD_NO;
		}

		state->postedName = NULL;
		state->postProcessor = NULL;

		if(strcmp(method, "POST") == 0) {
			state->postProcessor = MHD_create_post_processor(connection, 1024, postIterator, (void*)state);
			if(state->postProcessor == NULL) {
				free(state);
				return MHD_NO;
			}
		}
		*connectionContext = (void*)state;
		return MHD_YES;
	}

	struct ConnectionState * state = (struct ConnectionState *) * connectionContext;

	if(strcmp(url, "/meme") == 0) {
		if(strcmp(method, "GET") != 0) {
			return sendResponse(connection, 405, "Method Not Allowed");
		}
		return handleGetMeme(connection);
	}

	if(strcmp(url, "/add") == 0) {
		if(strcmp(method, "POST") != 0) {
			return sendResponse(connection, 405, "Method Not Allowed");
		}

		if(*uploadDataSize != 0) {
			MHD_post_process(state->postProcessor, uploadData, *uploadDataSize);
			*uploadDataSize = 0;
			return MHD_YES;
		}
		return handlePostAdd(connection, state);
	}

	return sendResponse(connection, 404, "Endpoint not found");
}

void startHttpServer(int port, sqlite3* dbConnection) {
	databaseConnection = dbConnection;
	serverDaemon = MHD_start_daemon(MHD_USE_EPOLL_INTERNALLY | MHD_USE_INTERNAL_POLLING_THREAD,
	                                port, NULL, NULL,
	                                &mainRequestHandler, NULL,
	                                MHD_OPTION_THREAD_POOL_SIZE, THREAD_POOL_CAPACITY,
	                                MHD_OPTION_NOTIFY_COMPLETED, requestCompleted, NULL,
	                                MHD_OPTION_END);
}

void stopHttpServer() {
	if(serverDaemon != NULL) {
		MHD_stop_daemon(serverDaemon);
		serverDaemon = NULL;
	}
}
