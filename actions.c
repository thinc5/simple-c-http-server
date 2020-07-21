#include <stdio.h>

#include <sys/socket.h>

#include "http.h"
#include "actions.h"

static const char *HTTP_OK_RESPONSE =
    "HTTP/1.1 200 OK\r\n"   // Status-Line
    "Connection: close\r\n" // Headers
    "Content-Length: 7\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"         // Content break
    "\n\n\tOk!\n"; // Content

static const char *HTTP_404_RESPONSE =
    "HTTP/1.1 404 Not Found\r\n"
    "Connection: close\r\n"
    "\r\n";

void ok_action(int client_socket, HTTP_REQUEST req)
{
    printf("Ok action!\n");

    if (req.body)
    {
        printf("We have a body\n%s\n", req.body);
    }

    // Action data.
    send(client_socket, HTTP_OK_RESPONSE,
         sizeof(HTTP_OK_RESPONSE), 0);
    printf("Replied:\n%s\n", HTTP_OK_RESPONSE);
}

void not_found_action(int client_socket, HTTP_REQUEST req)
{
    printf("Test filter action!\n");

    if (req.body)
    {
        printf("We have a body\n%s\n", req.body);
    }

    // Action data.
    send(client_socket, HTTP_404_RESPONSE,
         sizeof(HTTP_404_RESPONSE), 0);
    printf("Replied:\n%s\n", HTTP_404_RESPONSE);
}
