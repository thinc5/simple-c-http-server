#include <stdio.h>
#include <string.h>

#include "log.h"

#include "http.h"

#include "actions.h"
#include "filter.h"

void filter_request(int client_socket, HTTP_REQUEST req, HTTP_RESPONSE *res)
{
    DEBUG_LOG("------------ Filtering -------------\n");
    for (int i = 0; i < NUM_FILTERS; i++)
    {
        // Correct method?
        if (filters[i].method != req.method)
            continue;

        // Correct path?
        if (filters[i].path != NULL && req.path != NULL &&
            strcmp(filters[i].path, req.path) != 0)
            continue;

        // Correct host?
        // if (filters[i].host != NULL ||
        //      strcmp(filters[i].host, req.host) != 0)
        // {
        //     return;
        // }

        // Match headers.
        int match = filters[i].num_headers;
        // Iterate over required filter headers.
        for (int j = 0; j < filters[i].num_headers; j++)
        {
            // Iterate over the request itself.
            for (int k = 0; k < req.header_number; k++)
            {
                // Match the key.
                if (strcmp(filters[i].headers[j].key, req.headers[k].key) == 0)
                {
                    // Match the value.
                    if (strcmp(filters[i].headers[j].value,
                               req.headers[k].value) == 0)
                        match--;
                    break;
                }
            }
        }
        if (match == 0)
        {
            // Execute the action and exit the function.
            filters[i].action(client_socket, req);
            return;
        }
    }

    // No match, we return a generic 200.
    DEBUG_LOG("No matches, proceeding with standard 404\n");
    not_found_action(client_socket, req);
    DEBUG_LOG("------------------------------------!\n");
    return;
}
