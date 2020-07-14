#ifndef CONFIG_H
#define CONFIG_H

#include "actions.h"

#define LISTEN_ADDR "127.0.0.1"
#define PORT 8123

#define MAX_EVENTS 10
#define MAX_HEADERS 10

#define DEFAULT_PAYLOAD_SIZE 4096
#define PAYLOAD_MAX 16384

#define MINIMAL_HTTP_RESPONSE_HEADDER "HTTP/1.1 200 OK\r\nContent-Length: 7\r\nContent-Type: text/plain\r\n\n\n\tOk!\n\n"

typedef enum
{
    TEST_FILTER,
    POST_TEST,
    // DO NOT REMOVE THIS
    NUM_FILTERS,
} FILTERS;

typedef struct REQUEST_FILTER
{
    const HTTP_METHOD method;
    const char *path;
    const char *host;
    HTTP_HEADER headers[MAX_HEADERS];
    const int num_headers;
    void (*const action)(HTTP_REQUEST);
} REQUEST_FILTER;

// Filters will be defined here.
// First string is the method, path, version, then a list of desired headers and values,
// finally, the body will be passed to the action if needed.
static const REQUEST_FILTER filters[NUM_FILTERS] = {
    [TEST_FILTER] = {
        .method = GET,
        .path = "/",
        .host = NULL,
        .headers = {
            {.key = "Connection", .value = "keep-alive"},
        },
        .num_headers = 1,
        .action = &test_action,
    },
    [POST_TEST] = {
        .method = POST,
        .path = "/",
        .host = NULL,
        .headers = {
            {.key = "Connection", .value = "keep-alive"},
        },
        .num_headers = 1,
        .action = &test_action,
    },
};

#endif
