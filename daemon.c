/*
 * $Id: daemon.c 39 2010-11-02 23:42:57Z nicb $
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "psOSCd.h"

static void daemonize();

int
start_daemon()
{
    int result = 0;
    pid_t pid = 0;

    if (detach())
    {
      pid = fork();

	    if (pid == 0)
	    {
	        /* child process */
	        daemonize();
	        start_osc_services();
	    }
	    else if (pid < 0)
	        fatal_perror("fork");
    }
    else
      start_osc_services();

    return result;
}

static void
close_standard_stream(FILE *stream)
{
    if (fclose(stream) < 0)
        fatal_perror("fclose");
}

static void
daemonize()
{
    start_logging();
    atexit(stop_logging);

    close_standard_stream(stdin);
    close_standard_stream(stdout);
    close_standard_stream(stderr);
    initialize_signals();
}
