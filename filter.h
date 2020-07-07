#ifndef FILTER_H
#define FILTER_H

#include "http.h"

typedef enum
{
    METHOD,
    CLIENT,
    ACTION
} FILTER_COLUMN;

void filter_request(HTTP_REQUEST req);

#endif