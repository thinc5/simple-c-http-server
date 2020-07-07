#ifndef CONFIG_H
#define CONFIG_H

#include "actions.h"

#define LISTEN_ADDR "0.0.0.0"
#define PORT 8123

// Minimal http ok header to reply to those webhooks which require a response.
#define MINIMAL_HTTP_RESPONSE_HEADDER "HTTP/1.1 200 OK\r\nContent-Length: 7\r\nContent-Type: text/plain\r\n\n\n\tOk!\n\n"

typedef enum
{
    TEST_FILTER,
    // DO NOT REMOVE THIS
    NUM_FILTERS,
} FILTERS;

// Filters will be defined here. // see dwm config.h
static const char *filters[NUM_FILTERS][2] = {
    /*               method   client   */
    [TEST_FILTER] = {
        "POST",
        "0.0.0.0",
    },
};

// Filters will be defined here. // see dwm config.h
static const void (*actions[NUM_FILTERS])() = {
    /*             function pointer to action */
    [TEST_FILTER] = &test,
};

#endif
