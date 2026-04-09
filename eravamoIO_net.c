/*
 * This part of the code is written by a Large Language Model.
 * I didn't want to investigate microhttpd library for this project, as it is
 * meant to be just a fun project to laugh with my friends.
 */

#include <microhttpd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eravamoIO_net.h"
#include "eravamoIO_ops.h"
#include "eravamoIO_types.h"
#include "lib/sqlite3.h"

struct MHD_Daemon* serverDaemon = NULL;
sqlite3* databaseConnection = NULL;
pthread_mutex_t databaseMutex = PTHREAD_MUTEX_INITIALIZER;

struct ConnectionState {
    struct MHD_PostProcessor* postProcessor;
    char* postedName;
};

enum MHD_Result postIterator(void* connectionInfo, enum MHD_ValueKind kind,
                             const char* key, const char* filename,
                             const char* contentType,
                             const char* transferEncoding, const char* data,
                             uint64_t off, size_t size) {
    struct ConnectionState* state = (struct ConnectionState*)connectionInfo;

    if (strcmp(key, "name") == 0 && size > 0) {
        if (state->postedName == NULL) {
            state->postedName = malloc(size + 1);
            if (state->postedName != NULL) {
                memcpy(state->postedName, data, size);
                state->postedName[size] = '\0';
            }
        } else {
            size_t oldLen = strlen(state->postedName);
            char* newName = realloc(state->postedName, oldLen + size + 1);
            if (newName != NULL) {
                state->postedName = newName;
                memcpy(state->postedName + oldLen, data, size);
                state->postedName[oldLen + size] = '\0';
            }
        }
    }
    return MHD_YES;
                             }

                             void requestCompleted(void* cls, struct MHD_Connection* connection,
                                                   void** connectionContext,
                                                   enum MHD_RequestTerminationCode toe) {
                                 struct ConnectionState* state = (struct ConnectionState*)*connectionContext;
                                 if (state != NULL) {
                                     if (state->postProcessor != NULL) {
                                         MHD_destroy_post_processor(state->postProcessor);
                                     }
                                     if (state->postedName != NULL) {
                                         free(state->postedName);
                                     }
                                     free(state);
                                     connectionContext = NULL;
                                 }
                                                   }

                                                   enum MHD_Result sendResponse(struct MHD_Connection* connection, int statusCode,
                                                                                const char* content) {
                                                       struct MHD_Response* response = MHD_create_response_from_buffer(
                                                           strlen(content), (void*)content, MHD_RESPMEM_MUST_COPY);
                                                       if (response == NULL) {
                                                           return MHD_NO;
                                                       }

                                                       MHD_add_response_header(response, "Content-Type",
                                                                               "text/plain; charset=utf-8");
                                                       MHD_add_response_header(response, "X-Content-Type-Options", "nosniff");

                                                       enum MHD_Result result = MHD_queue_response(connection, statusCode, response);
                                                       MHD_destroy_response(response);

                                                       return result;
                                                                                }

                                                                                enum MHD_Result handleGetMeme(struct MHD_Connection* connection) {
                                                                                    const char* countParameter =
                                                                                    MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "count");
                                                                                    int numberOfNames = 3;

                                                                                    if (countParameter != NULL) {
                                                                                        numberOfNames = atoi(countParameter);
                                                                                        if (numberOfNames < 3) {
                                                                                            numberOfNames = 3;
                                                                                        } else if (numberOfNames > 50) {
                                                                                            numberOfNames = 50;
                                                                                        }
                                                                                    }

                                                                                    pthread_mutex_lock(&databaseMutex);
                                                                                    StringArray retrievedNames =
                                                                                    eravamoIO_retrieveNames(databaseConnection, numberOfNames);
                                                                                    pthread_mutex_unlock(&databaseMutex);

                                                                                    if (retrievedNames.len < 3) {
                                                                                        cleanStringArray(&retrievedNames);
                                                                                        return sendResponse(connection, 400, "Not enough names");
                                                                                    }

                                                                                    size_t bufferSize = strlen("Eravamo IO") + 2;
                                                                                    for (size_t i = 0; i < retrievedNames.len; i++) {
                                                                                        bufferSize += strlen(retrievedNames.data[i]) + 2;
                                                                                    }

                                                                                    char* responseContent = malloc(bufferSize);
                                                                                    if (responseContent == NULL) {
                                                                                        cleanStringArray(&retrievedNames);
                                                                                        return sendResponse(connection, 500, "Internal Server Error");
                                                                                    }

                                                                                    strcpy(responseContent, "Eravamo IO");
                                                                                    for (size_t i = 0; i < retrievedNames.len; i++) {
                                                                                        strcat(responseContent, ", ");
                                                                                        strcat(responseContent, retrievedNames.data[i]);
                                                                                    }
                                                                                    strcat(responseContent, ".");

                                                                                    enum MHD_Result result = sendResponse(connection, 200, responseContent);
                                                                                    free(responseContent);
                                                                                    cleanStringArray(&retrievedNames);

                                                                                    return result;
                                                                                }

                                                                                enum MHD_Result handlePostAdd(struct MHD_Connection* connection,
                                                                                                              struct ConnectionState* state) {
                                                                                    if (state->postedName == NULL) {
                                                                                        return sendResponse(connection, 400, "Missing name parameter");
                                                                                    }

                                                                                    char* sanitizedName = sanityCheck(state->postedName);
                                                                                    if (sanitizedName == NULL) {
                                                                                        return sendResponse(connection, 400, "Invalid name format");
                                                                                    }

                                                                                    pthread_mutex_lock(&databaseMutex);
                                                                                    eravamoIO_addName(databaseConnection, sanitizedName);
                                                                                    pthread_mutex_unlock(&databaseMutex);

                                                                                    return sendResponse(connection, 200, "Name added successfully");
                                                                                                              }

                                                                                                              enum MHD_Result mainRequestHandler(void* cls, struct MHD_Connection* connection,
                                                                                                                                                 const char* url, const char* method,
                                                                                                                                                 const char* version, const char* uploadData,
                                                                                                                                                 size_t* uploadDataSize,
                                                                                                                                                 void** connectionContext) {
                                                                                                                  if (*connectionContext == NULL) {
                                                                                                                      struct ConnectionState* state = malloc(sizeof(struct ConnectionState));
                                                                                                                      if (state == NULL) {
                                                                                                                          return MHD_NO;
                                                                                                                      }

                                                                                                                      state->postedName = NULL;
                                                                                                                      state->postProcessor = NULL;

                                                                                                                      if (strcmp(method, "POST") == 0) {
                                                                                                                          state->postProcessor = MHD_create_post_processor(
                                                                                                                              connection, 1024, postIterator, (void*)state);
                                                                                                                          if (state->postProcessor == NULL) {
                                                                                                                              free(state);
                                                                                                                              return MHD_NO;
                                                                                                                          }
                                                                                                                      }

                                                                                                                      *connectionContext = (void*)state;
                                                                                                                      return MHD_YES;
                                                                                                                  }

                                                                                                                  struct ConnectionState* state = (struct ConnectionState*)*connectionContext;

                                                                                                                  if (strcmp(method, "POST") == 0) {
                                                                                                                      if (*uploadDataSize != 0) {
                                                                                                                          MHD_post_process(state->postProcessor, uploadData, *uploadDataSize);
                                                                                                                          *uploadDataSize = 0;
                                                                                                                          return MHD_YES;
                                                                                                                      }
                                                                                                                  }

                                                                                                                  if (strcmp(method, "GET") == 0 && strcmp(url, "/meme") == 0) {
                                                                                                                      return handleGetMeme(connection);
                                                                                                                  }

                                                                                                                  if (strcmp(method, "POST") == 0 && strcmp(url, "/add") == 0) {
                                                                                                                      return handlePostAdd(connection, state);
                                                                                                                  }

                                                                                                                  return sendResponse(connection, 404, "Endpoint not found");
                                                                                                                                                 }

                                                                                                                                                 void startHttpServer(int port, sqlite3* dbConnection) {
                                                                                                                                                     databaseConnection = dbConnection;
                                                                                                                                                     serverDaemon = MHD_start_daemon(
                                                                                                                                                         MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_THREAD_PER_CONNECTION, port,
                                                                                                                                                         NULL, NULL, &mainRequestHandler, NULL, MHD_OPTION_NOTIFY_COMPLETED,
                                                                                                                                                         requestCompleted, NULL, MHD_OPTION_END);
                                                                                                                                                 }

                                                                                                                                                 void stopHttpServer() {
                                                                                                                                                     if (serverDaemon != NULL) {
                                                                                                                                                         MHD_stop_daemon(serverDaemon);
                                                                                                                                                         serverDaemon = NULL;
                                                                                                                                                     }
                                                                                                                                                 }
