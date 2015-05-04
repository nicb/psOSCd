/*
 * $Id: signals.c 39 2010-11-02 23:42:57Z nicb $
 */

#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "psOSCd.h"

/*
 * When used in debugging mode, this initializes all signals so that they will
 * result in an abort() call, in order to catch malfunctionings in the daemon
 * process. This call should be executed *after* the daemon fork.
 */
static void initialize_debug_signals();

void
initialize_signals()
{
    initialize_debug_signals();
}

/*
 * Used only in debugging
 */

#if PHASESPACE_DEBUG == 1
static void phasespace_signal_handler(int signum, siginfo_t *info, void *ptr);
static const char *signal_map(int signum, char buf[]);

typedef struct _PhaseSpaceSignal_
{
    int signal;
    const char desc[PSOSCD_SMALL_BUFSIZE];
    struct sigaction action;
} PhaseSpaceSignal;

static PhaseSpaceSignal signals[] =
{
    { SIGHUP,  { "HUP" },  {{ '\0' }} },
    /* { SIGINT,  { "INT" },  {{ '\0' }} }, */
    { SIGQUIT, { "QUIT" }, {{ '\0' }} },
    { SIGFPE,  { "FPE" },  {{ '\0' }} },
    { SIGSEGV, { "SEGV" }, {{ '\0' }} },
    { SIGPIPE, { "PIPE" }, {{ '\0' }} },
    { SIGALRM, { "ALRM" }, {{ '\0' }} },
    { SIGTERM, { "TERM" }, {{ '\0' }} },
    { SIGCHLD, { "CHLD" }, {{ '\0' }} },
    { SIGURG,  { "URG" },  {{ '\0' }} },
    { SIGXCPU, { "XCPU" }, {{ '\0' }} },
    { SIGXFSZ, { "XFSZ" }, {{ '\0' }} },
    { -1,    { '\0' }, {{ '\0' }} }         /* loop terminator */
};

static void
initialize_debug_signals()
{
    PhaseSpaceSignal *sig = signals;

    psOSCd_debug("start installing signal handlers");

    while (sig->signal != -1)
    {
        char buf[PSOSCD_SMALL_BUFSIZE] = { '\0' };
        int res = 0;

        sig->action.sa_sigaction = phasespace_signal_handler;
        sig->action.sa_flags = SA_SIGINFO;

        if ((res = sigaction(sig->signal, &sig->action, (struct sigaction *) NULL)) < 0)
            fatal_perror("sigaction (signal %s)", sig->desc);

        psOSCd_debug("installed signal handler for signal %s", signal_map(sig->signal, buf));

        ++sig;
    }

    psOSCd_debug("done installing signal handlers");
}

static void
phasespace_signal_handler(int signum, siginfo_t *info, void *ptr)
{
    char buf[PSOSCD_SMALL_BUFSIZE] = { '\0' };
    psOSCd_debug("received signal %d (SIG%s), dumping core...", signum, signal_map(signum, buf));
    abort();
}

static const char *
signal_map(int signum, char buf[])
{
    const PhaseSpaceSignal *sig = signals;

    strcpy(buf, "UNKOWN");
    while (sig->signal != -1)
    {
        if (signum == sig->signal)
        {
            strcpy(buf, sig->desc);
            break;
        }
        ++sig;
    }

    return buf;
}
#else
static void initialize_debug_signals(){};
#endif /* PHASESPACE_DEBUG=1 */
