#ifndef CONFIG_H
#define CONFIG_H

#include "actions.h"

#define LISTEN_ADDR "127.0.0.1"
#define PORT 8123

#define MAX_EVENTS 10

#define DEFAULT_PAYLOAD_SIZE 4096
#define PAYLOAD_MAX 16384

#define MINIMAL_HTTP_RESPONSE_HEADDER "HTTP/1.1 200 OK\r\nContent-Length: 7\r\nContent-Type: text/plain\r\n\n\n\tOk!\n\n"

typedef enum
{
    TEST_FILTER,
    // DO NOT REMOVE THIS
    NUM_FILTERS,
} FILTERS;

// Filters will be defined here. // see dwm config.h
static const char *const filters[NUM_FILTERS][2] = {
    /*               method   client   */
    [TEST_FILTER] = {
        "POST",
        "0.0.0.0",
    },
};

// Filters will be defined here. // see dwm config.h
static const void (*const actions[NUM_FILTERS])() = {
    /*             function pointer to action */
    [TEST_FILTER] = &test,
};

#endif
