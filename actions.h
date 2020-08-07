#ifndef ACTIONS_H
#define ACTIONS_H

#include "http.h"

void generic_action(int client_socket, HTTP_REQUEST req);

void ok_action(int client_socket, HTTP_REQUEST req);

void not_found_action(int client_socket, HTTP_REQUEST req);

void sample_github_action(int client_socket, HTTP_REQUEST req);

void update_server_action(int client_socket, HTTP_REQUEST req);

#endif
