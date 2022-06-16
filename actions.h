#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdbool.h>

#include "http.h"

bool generic_action(int client_socket, HTTP_REQUEST req);

bool ok_action(int client_socket, HTTP_REQUEST req);

bool not_found_action(int client_socket, HTTP_REQUEST req);

bool sample_github_action(int client_socket, HTTP_REQUEST req);

bool update_server_action(int client_socket, HTTP_REQUEST req);

#endif
