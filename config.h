#ifndef CONFIG_H
#define CONFIG_H

#define LISTEN_ADDR "0.0.0.0"
#define PORT 8123

// Minimal http ok header to reply to those webhooks which require a response.
#define MINIMAL_HTTP_RESPONSE_HEADDER "HTTP/1.1 200 OK\r\nContent-Length: 7\r\nContent-Type: text/plain\r\n\n\n\tOk!\n\n"

static const void
test()
{
    printf("Test filter action!\n");
}

typedef enum
{
    test_filter,
} FILTERS;

// Filters will be defined here. // see dwm config.h
static const char *filters[][3] = {
    /*               method   client      action */
    [test_filter] = {"POST", "github.com", &test},
};

#endif
