#ifndef ACTIONS_H
#define ACTIONS_H

#include "http.h"

void generic_action(int client_socket, HTTP_REQUEST req);

void ok_action(int client_socket, HTTP_REQUEST req);

void not_found_action(int client_socket, HTTP_REQUEST req);

void github_action(int client_socket, HTTP_REQUEST req);

#endif
