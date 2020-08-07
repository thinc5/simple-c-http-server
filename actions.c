#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include "log.h"

#include "http.h"
#include "actions.h"

#include "./utf-8/utf.h"

static const char *HTTP_OK_RESPONSE =
    "HTTP/1.1 204 OK\r\n"   // Status-Line
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

static const char *HTTP_500_RESPONSE =
    "HTTP/1.1 500 Internal Server Error\r\n"
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

// A generic 204, let the client know that we accepted the message, but there
// is no content.
void ok_action(int client_socket, HTTP_REQUEST req)
{
    if (req.body)
    {
        DEBUG_LOG("We have a body\n%s\n", req.body);
    }

    // Action data.
    send(client_socket, HTTP_OK_TEXT_RESPONSE,
         strlen(HTTP_OK_TEXT_RESPONSE), 0);
}

// Error processing or no match, simply a 404.
void not_found_action(int client_socket, HTTP_REQUEST req)
{
    DEBUG_LOG("404 action!!\n");

    // Action data.
    send(client_socket, HTTP_404_RESPONSE,
         strlen(HTTP_404_RESPONSE), 0);
}

void sample_github_action(int client_socket, HTTP_REQUEST req)
{
    static const char *repo_id = "276006277";
    static const char *script_path = "../scripts/update_http_server.sh";

    DEBUG_LOG("GitHub Webhook action for this project!\n");


    // Parse the body, if it does not exist (not valid POST) return.
    if (!req.body)
    {
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }

    char *uncoded_body = (char *)malloc(sizeof(char *) * strlen(req.body));
    convert_from_utf8(req.body, uncoded_body);
    const char *needle = "repository\":{\"id\":";
    char *id = strstr(uncoded_body, needle);
    if (id == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    id += strlen(needle);
    char *end = strchr(id, ',');
    if (end == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    end[0] = '\0';

    if (strcmp(id, repo_id) != 0)
    {
        DEBUG_LOG("GitHub id %s does not match %s\n", id, repo_id);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    free(uncoded_body);

    // Everything is good so far, perform the actual "action".
    if (!file_exists(script_path))
    {
        ERROR_LOG("File %s not found\n", script_path);
        send(client_socket, HTTP_500_RESPONSE,
             strlen(HTTP_500_RESPONSE), 0);
        return;
    }
    
    int pid = fork();
    if (pid == 0)
    {
        execl("/bin/sh", "sh", script_path, NULL);
        // If exec fails we terminate the process.
        kill(getpid(), SIGKILL);
    }
    DEBUG_LOG("Success! Letting GitHub know!\n");
    send(client_socket, HTTP_OK_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);
}

void update_server_action(int client_socket, HTTP_REQUEST req)
{
    static const char *repo_id = "276006277";
    static const char *script_path = "../scripts/update_http_server.sh";

    DEBUG_LOG("GitHub Webhook action for this project!\n");

    // Parse the body, if it does not exist (not valid POST) return.
    if (!req.body)
    {
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }

    char *uncoded_body = (char *)malloc(sizeof(char *) * strlen(req.body));
    convert_from_utf8(req.body, uncoded_body);
    const char *needle = "repository\":{\"id\":";
    char *id = strstr(uncoded_body, needle);
    if (id == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    id += strlen(needle);
    char *end = strchr(id, ',');
    if (end == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    end[0] = '\0';

    if (strcmp(id, repo_id) != 0)
    {
        DEBUG_LOG("GitHub id %s does not match %s\n", id, repo_id);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    free(uncoded_body);

    // Everything is good so far, perform the actual "action".
    if (!file_exists(script_path))
    {
        ERROR_LOG("File %s not found\n", script_path);
        send(client_socket, HTTP_500_RESPONSE,
             strlen(HTTP_500_RESPONSE), 0);
        return;
    }
    
    DEBUG_LOG("Success! Letting GitHub know!\n");
    send(client_socket, HTTP_OK_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);

    // Not great since we don't clean up but the alternative is a callback.
    execl("/bin/sh", "sh", script_path, NULL);
}

void update_website_action(int client_socket, HTTP_REQUEST req)
{
    static const char *web_repo_id = "275560807";
    static const char *blog_repo_id = "275565965";
    static const char *script_path = "../scripts/update_website.sh";

    DEBUG_LOG("GitHub Webhook action for website!\n");

    // Parse the body, if it does not exist (not valid POST) return.
    if (!req.body)
    {
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }

    char *uncoded_body = (char *)malloc(sizeof(char *) * strlen(req.body));
    convert_from_utf8(req.body, uncoded_body);
    const char *needle = "repository\":{\"id\":";
    char *id = strstr(uncoded_body, needle);
    if (id == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    id += strlen(needle);
    char *end = strchr(id, ',');
    if (end == NULL)
    {
        free(uncoded_body);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    end[0] = '\0';

    if (strcmp(id, web_repo_id) != 0 && strcmp(id, blog_repo_id) != 0)
    {
        DEBUG_LOG("GitHub id %s does not match provided repository ids\n", id);
        send(client_socket, HTTP_404_RESPONSE,
             strlen(HTTP_404_RESPONSE), 0);
        return;
    }
    free(uncoded_body);

    // Everything is good so far, perform the actual "action".
    if (!file_exists(script_path))
    {
        ERROR_LOG("File %s not found\n", script_path);
        send(client_socket, HTTP_500_RESPONSE,
             strlen(HTTP_500_RESPONSE), 0);
        return;
    }
    
    int pid = fork();
    if (pid == 0)
    {
        execl("/bin/sh", "sh", script_path, NULL);
        // If exec fails we terminate the process.
        kill(getpid(), SIGKILL);
    }
    DEBUG_LOG("Success! Letting GitHub know!\n");
    send(client_socket, HTTP_OK_RESPONSE,
         strlen(HTTP_OK_RESPONSE), 0);
}


