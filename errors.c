/*
 * $Id: errors.c 44 2012-08-01 23:26:55Z nicb $
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "psOSCd.h"

#define FATAL_EXITCODE (-1)

int
warning(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    _psOSCd_log(fmt, ap);

    return 0;
}

int
_fatal(const char *fmt, va_list ap)
{
    int result = FATAL_EXITCODE;
		va_list ap2;

		va_copy(ap2, ap);
    _psOSCd_log(fmt, ap2);
    exit(FATAL_EXITCODE);

    return result;
}

int
fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    return _fatal(fmt, ap);
}

int
fatal_perror(const char *fmt, ...)
{
    const int bufsize = PSOSCD_BUFSIZE;
    char error_buf[bufsize];
    char message_buf[bufsize];
    va_list ap;

    va_start(ap, fmt);
    strerror_r(errno, error_buf, bufsize);
    vsprintf(message_buf, fmt, ap);

    return fatal("%s: %s", message_buf, error_buf);
}
