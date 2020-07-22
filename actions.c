#include <stdio.h>
#include <string.h>

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
         strlen(HTTP_OK_RESPONSE), 0);
}

void not_found_action(int client_socket, HTTP_REQUEST req)
{
    printf("404 action!!\n");

    // Action data.
    send(client_socket, HTTP_404_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);
}

void github_action(int client_socket, HTTP_REQUEST req)
{
    printf("GitHub Webhook action!\n");

    // Would parse the body here.
    if (req.body)
    {
        printf("We have a body\n%s\n", req.body);
    }

    // Action data.
    send(client_socket, HTTP_OK_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);
}
