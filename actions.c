#include <stdio.h>

#include "http.h"
#include "actions.h"

void test_action(HTTP_REQUEST req)
{
    if (req.body)
    {
        printf("We have a body\n%s\n", req.body);
    }
    printf("Test filter action!\n");
}
