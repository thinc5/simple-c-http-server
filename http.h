#ifndef HTTP_H
#define HTTP_H

typedef enum HTTP_PARSE_ERRORS
{
    OK,
    REFERENCE_ERROR,
    UNSUPPORTED_VERSION,
    UNKNOWN_DATA,
    INVALID_METHOD,
    PARSE_ERROR_NUM,
} HTTP_PARSE_ERRORS;

static const char *const HTTP_PARSE_ERROR_STRINGS[PARSE_ERROR_NUM] = {
    "OK",
    "REFERENCE_ERROR",
    "UNSUPPORTED_VERSION",
    "UNKNOWN_DATA",
    "INVALID_METHOD",
};

typedef enum HTTP_VERSION
{
    UNSUPPORTED,
    HTTP1,
    HTTP11,
    HTTP2,
    SUPPORTED_VERSIONS,
} HTTP_VERSION;

static const char *const HTTP_VERSION_STRING[SUPPORTED_VERSIONS] = {
    "UNSUPPORTED_VERSION",
    "HTTP/1",
    "HTTP/1.1",
    "HTTP/2",
};

typedef enum HTTP_METHOD
{
    INV_METHOD,
    GET,
    POST,
    UPDATE,
    PATCH,
    OPTIONS,
    CONNECT,
    HEAD,
    NUM_METHODS,
} HTTP_METHOD;

static const char *const HTTP_METHOD_STRING[NUM_METHODS] = {
    "INVALID METHOD",
    "GET",
    "POST",
    "UPDATE",
    "PATCH",
    "OPTIONS",
    "CONNECT",
    "HEAD",
};

typedef enum HTTP_POSITION
{
    START_LINE,
    HEADERS,
    BODY,
    DONE
} HTTP_POSITION;

typedef struct HTTP_HEADER
{
    char *key;
    char *value;
} HTTP_HEADER;

typedef struct HTTP_REQUEST
{
    HTTP_VERSION version;
    HTTP_METHOD method;
    char *path;
    HTTP_HEADER *headers;
    unsigned int header_number;
    char *body;
} HTTP_REQUEST;

typedef struct HTTP_RESPONSE
{
    HTTP_VERSION version;
    HTTP_HEADER *headers;
    unsigned int header_number;
    char *body;
} HTTP_RESPONSE;

HTTP_PARSE_ERRORS
parse_http_request(HTTP_REQUEST *req, char *raw);

void free_http_request(HTTP_REQUEST *req);

void show_http_request(HTTP_REQUEST *req);

const char *get_http_header_value(HTTP_REQUEST req, const char *key);

HTTP_PARSE_ERRORS
parse_http_response(HTTP_RESPONSE *res, char *raw);

#endif
