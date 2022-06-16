#ifndef FILTER_H
#define FILTER_H

#include <stdbool.h>

#include "config.h"
#include "actions.h"
#include "http.h"

typedef enum FILTER_COLUMN
{
    METHOD,
    PATH,
    CLIENT,
    ACTION
} FILTER_COLUMN;

typedef enum FILTERS
{
    TEST_FILTER,
    UPDATE_SERVER,
    GITHUB_EXAMPLE,
    NUM_FILTERS,
} FILTERS;

typedef struct REQUEST_FILTER
{
    const HTTP_METHOD method;
    const char *path;
    const char *host;
    HTTP_HEADER headers[MAX_HEADERS];
    const int num_headers;
    bool (*const action)(int, HTTP_REQUEST);
} REQUEST_FILTER;

// Filters will be defined here.
// First string is the method, path, version, then a list of desired headers
// and values, finally, the body will be passed to the action if needed.
static const REQUEST_FILTER filters[] = {
    [TEST_FILTER] = {
        .method = GET,
        .path = "/",
        .host = NULL,
        .num_headers = 0,
        .action = ok_action,
    }, 
    [UPDATE_SERVER] = {
        .method = POST,
        .path = "/",
        .host = NULL,
        .headers = {
            {"x-github-event", "push"},
            {"content-type", "application/x-www-form-urlencoded"},
        },
        .num_headers = 2,
        .action = update_server_action,
    },
    [GITHUB_EXAMPLE] = {
        .method = POST,
        .path = "/",
        .host = NULL,
        .headers = {
            {"x-github-event", "push"},
            {"content-type", "application/x-www-form-urlencoded"},
        },
        .num_headers = 2,
        .action = sample_github_action,
   }
};

void filter_request(int client_socket, HTTP_REQUEST req, HTTP_RESPONSE *res);

#endif
