#ifndef ACTIONS_H
#define ACTIONS_H

#include "http.h"

void ok_action(int client_socket, HTTP_REQUEST req);

void not_found_action(int client_socket, HTTP_REQUEST req);

#endif
