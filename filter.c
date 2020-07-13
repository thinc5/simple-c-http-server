#include <stdio.h>
#include <string.h>

#include "http.h"
#include "config.h"

#include "filter.h"

void filter_request(HTTP_REQUEST req)
{
    for (int i = 0; i < NUM_FILTERS; i++)
    {
        // TODO: Correct client?
        // if (strcmp(filters[i][CLIENT], req.method))
        // Correct method?
        if (strcmp(filters[i][METHOD], HTTP_METHOD_STRING[req.method]) == 0)
        {
            // Call the provided function.
            actions[i]();
        }
    }
}
