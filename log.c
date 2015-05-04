/*
 * $Id: log.c 44 2012-08-01 23:26:55Z nicb $
 */

#include <syslog.h>
#include <stdio.h>

#include "psOSCd.h"

static void
common_log(int severity, const char *fmt, va_list ap)
{
		va_list ap2;

		va_copy(ap2, ap);
    vsyslog(severity, fmt, ap2);
}

void
psOSCd_debug(const char *fmt, ...)
{
    if (verbose())
    {
        va_list ap;
        va_start(ap, fmt);

        common_log(LOG_INFO, fmt, ap);
    }
}

void
_psOSCd_log(const char *fmt, va_list ap)
{
		va_list ap2;

		va_copy(ap2, ap);
    common_log(LOG_INFO, fmt, ap2);
}

void
psOSCd_log(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    _psOSCd_log(fmt, ap);
}

void
start_logging()
{
    const char *ident = program_name();
    int logopt = LOG_PID | LOG_CONS;
    int facility = LOG_USER;
    openlog(ident, logopt, facility);

    psOSCd_log("psOSCd logging started\n");
}

void
stop_logging()
{
    psOSCd_log("psOSCd logging stopped\n");
    closelog();
}

void
set_debug_log()
{
    setlogmask(LOG_DEBUG);
}
