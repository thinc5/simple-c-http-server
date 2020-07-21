#include <stdio.h>
#include <string.h>

#include "http.h"

#include "filter.h"

void filter_request(int client_socket, HTTP_REQUEST req, HTTP_RESPONSE *res)
{
    printf("----------- Filtering -----------\n");
    for (int i = 0; i < NUM_FILTERS; i++)
    {
        // Correct method?
        if (filters[i].method != req.method)
            return;

        // Correct path?
        if (filters[i].path != NULL && req.path != NULL &&
            strcmp(filters[i].path, req.path) != 0)
            return;

        // Correct host?
        // if (filters[i].host != NULL ||
        //      strcmp(filters[i].host, req.host) != 0)
        // {
        //     return;
        // }

        // Match headers.
        int match = filters[i].num_headers;
        for (int j = 0; j < filters[i].num_headers; j++)
        {
            for (int k = 0; k < req.header_number; k++)
            {
                if (strcmp(filters[i].headers[j].key, req.headers[k].key) == 0)
                {
                    if (strcmp(filters[i].headers[j].value,
                               req.headers[k].value) == 0)
                    {
                        match--;
                        break;
                    }
                    return;
                }
            }
        }
        if (match == 0)
        {
            filters[i].action(client_socket, req);
        }
    }
}
