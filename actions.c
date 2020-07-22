#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include "log.h"

#include "http.h"
#include "actions.h"

static const char *HTTP_OK_RESPONSE =
    "HTTP/1.1 200 OK\r\n"   // Status-Line
    "Connection: close\r\n" // Headers
    "\r\n";                 // Content break

static const char *HTTP_OK_TEXT_RESPONSE =
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

static const inline int file_exists(const char *path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void generic_action(int client_socket, HTTP_REQUEST req)
{
    // Action data.
    send(client_socket, HTTP_OK_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);
}

void ok_action(int client_socket, HTTP_REQUEST req)
{
    DEBUG_LOG("Ok action!\n");

    if (req.body)
    {
        DEBUG_LOG("We have a body\n%s\n", req.body);
    }

    // Action data.
    send(client_socket, HTTP_OK_TEXT_RESPONSE,
         strlen(HTTP_OK_TEXT_RESPONSE), 0);
}

void not_found_action(int client_socket, HTTP_REQUEST req)
{
    DEBUG_LOG("404 action!!\n");

    // Action data.
    send(client_socket, HTTP_404_RESPONSE,
         strlen(HTTP_404_RESPONSE), 0);
}

void github_action(int client_socket, HTTP_REQUEST req)
{
    static const char *script_path = "./update_self.sh";

    DEBUG_LOG("GitHub Webhook action!\n");

    // Would parse the body here.
    if (req.body)
        DEBUG_LOG("We have a body\n%s\n", req.body);

    // Perform the actual "action".
    if (file_exists(script_path))
    {
        int pid = fork();
        if (pid == 0)
        {
            DEBUG_LOG("Child process!\n");
            close(1);
            close(2);
            execl("/bin/sh", "sh", script_path, NULL);
            // If exec fails we terminate the process.
            kill(getpid(), SIGKILL);
        }
    }
    else
    {
        DEBUG_LOG("Error: file %s not found\n", script_path);
    }
    DEBUG_LOG("Child should not get here....\n");
    // Action data.
    send(client_socket, HTTP_OK_TEXT_RESPONSE,
         strlen(HTTP_OK_TEXT_RESPONSE), 0);
}
