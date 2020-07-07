#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "http.h"

/**
 *  Attempt to parse the http request start line.
 */
static HTTP_PARSE_ERRORS
parse_request_start_line(HTTP_REQUEST *req, char *start_line)
{
    char *saveptr = NULL;
    char *start_slice = strtok_r(start_line, " ", &saveptr);
    // HTTP method
    for (HTTP_METHOD method = GET; method <= NUM_METHODS; method++)
    {
        if (req->method == NUM_METHODS)
        {
            return INVALID_METHOD;
        }
        if (strcmp(start_slice, HTTP_METHOD_STRING[method]) == 0)
        {
            req->method = method;
            break;
        }
    }
    // printf("Method: %s\n", HTTP_METHOD_STRING[req->method]);
    // Path
    start_slice = strtok_r(NULL, " ", &saveptr);
    req->path = strdup(start_slice);
    // Version
    start_slice = strtok_r(NULL, " ", &saveptr);
    for (HTTP_VERSION ver = HTTP11; ver <= SUPPORTED_VERSIONS; ver++)
    {
        if (ver == SUPPORTED_VERSIONS)
        {
            return UNSUPPORTED_VERSION;
        }
        if (strcmp(start_slice, HTTP_VERSION_STRING[ver]) == 0)
        {
            req->version = ver;
            break;
        }
    }
    return OK;
}

/**
 * Given a string attempt to parse a http header, (key and value)
 */
static bool
parse_http_header(HTTP_HEADER *hdr, char *header_line)
{
    // Should be able to split each line in two, separated with ": "
    hdr->key = NULL;
    hdr->value = NULL;
    char *saveptr = NULL;
    char *buff = NULL;
    // Grab the section before the ": "
    buff = strtok_r(header_line, ":\\ ", &saveptr);
    hdr->key = strdup(buff);
    // Grab the section after the ": "
    buff = strtok_r(NULL, "\r\n", &saveptr);
    // It appears we need to skip a byte, since the token we get has the space from
    // when we did the original token...
    buff++;
    hdr->value = strdup(buff);
    return true;
}

/**
 * Display the contents of a parsed http header struct for debug purposes.
 */
void show_http_request(HTTP_REQUEST *req)
{
    printf("----- HTTP Request -----\n");
    printf("Version: %s\nMethod: %s\nPath: %s\n",
           HTTP_VERSION_STRING[req->version], HTTP_METHOD_STRING[req->method], req->path);
    if (req->header_number)
        printf("--- Headers ---\n");
    for (int i = 0; i < req->header_number; i++)
    {
        printf("%s -> %s\n", req->headers[i].key, req->headers[i].value);
    }
    if (req->body)
    {
        printf("--- Body ---\n%s\n", req->body);
    }
    printf("----------------\n");
}

/**
 * Free the http request struct.
 */
void free_http_request(HTTP_REQUEST *req)
{
    if (req->path)
    {
        free(req->path);
        req->path = NULL;
    }
    for (int i = 0; i < req->header_number; i++)
    {
        free(req->headers[i].key);
        req->headers[i].key = NULL;
        free(req->headers[i].value);
        req->headers[i].value = NULL;
    }
    if (req->headers)
    {
        free(req->headers);
        req->headers = NULL;
    }
    if (req->body)
    {
        free(req->body);
        req->body = NULL;
    }
    req->header_number = 0;
}

HTTP_PARSE_ERRORS
parse_http_request(HTTP_REQUEST *req, char *raw)
{
    HTTP_PARSE_ERRORS status = OK;
    // Are our pointers valid?
    if (req == NULL || req == NULL)
    {
        return REFERENCE_ERROR;
    }
    // Expected position in request body.
    HTTP_POSITION http_position = START_LINE;
    // Split our raw character buffer line by line. (\r\n)
    // Maintain the original string by creating a local copy.
    char *canonical_raw = strdup(raw);
    char *saveptr = NULL;
    char *slice = strtok_r(canonical_raw, "\r\n", &saveptr);
    unsigned int num_headers = 0;
    unsigned int parsed_headers = 0;
    char *body = NULL;
    while (slice != NULL && http_position != DONE)
    {
        switch (http_position)
        {
        case START_LINE:
            // Parse the request header.
            status = parse_request_start_line(req, slice);
            break;
        case HEADERS:
            // Get -n headers.
            if (!parse_http_header(&req->headers[parsed_headers], slice))
            {
                status = UNKNOWN_DATA;
                break;
            }
            parsed_headers++;
            if (parsed_headers == num_headers)
            {
                http_position++;
                break;
            }
            break;
        case BODY:
            // Regardless if there is a body, we must end here.
            http_position++;
            // Extract the body from the original raw data.
            body = strstr(raw, "\r\n\r\n") + strlen("\r\n\r\n");
            if (body == NULL)
            {
                break;
            }
            req->body = strdup(body);

            break;
        default:
            break;
        }
        // If we are still going after reading the first line, lets check how many headers we have!
        if (http_position == START_LINE)
        {
            char *backupptr = NULL;
            char *local_raw = strndup(raw, strlen(raw));
            // printf("raw:\n%s\n", local_raw);
            char *header_slice = strtok_r(local_raw, "\r\n", &backupptr);
            while (header_slice != NULL && strcmp(header_slice, "\n") != 0)
            {
                num_headers++;
                header_slice = strtok_r(NULL, "\r", &backupptr);
            };
            free(local_raw);
            num_headers--;
            // Now we allocate the memory for the headers.
            req->headers = (HTTP_HEADER *)malloc(sizeof(HTTP_HEADER) * num_headers);
            req->header_number = num_headers;
            http_position++;
        }

        // Have we encountered errors?
        if (status != OK)
        {
            free(canonical_raw);
            free_http_request(req);
            printf("Status: %d\n", status);
            return status;
        }
        // Get the next line.
        slice = strtok_r(NULL, "\r\n", &saveptr);
    }

    free(canonical_raw);
    return OK;
}

HTTP_PARSE_ERRORS
parse_http_response(HTTP_RESPONSE *res, char *raw)
{
    return OK;
}
