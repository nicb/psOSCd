/*
 * $Id: safe_malloc.c 36 2010-10-04 16:40:21Z nicb $
 */

#include <stdlib.h>

#include "psOSCd.h"

void *
safe_malloc(size_t size)
{
    void *result = malloc(size);

    if (result == NULL)
        fatal_perror("malloc for size %d", size);

    return result;
}
