/*
 * $Id: psOSCd.h 44 2012-08-01 23:26:55Z nicb $
 */

#if !defined(_PSOSCD_H_)
#   define _PSOSCD_H_

#if !defined(PSOSCD_BUFSIZE)
#   define PSOSCD_BUFSIZE (4096)
#endif /* !defined(PSOSCD_BUFSIZE) */

#if !defined(PSOSCD_SMALL_BUFSIZE)
#   define PSOSCD_SMALL_BUFSIZE (128)
#endif /* !defined(PSOSCD_SMALL_BUFSIZE) */

#if !defined(PSOSCD_DEFAULT_CONFIGURATION_FILENAME)
#   define PSOSCD_DEFAULT_CONFIGURATION_FILENAME "/etc/psoscd/psoscd.conf"
#endif /* !defined(PSOSCD_DEFAULT_CONFIGURATION_FILENAME) */

#include <unistd.h>
#include <stdarg.h>

#include "phasespace.h"
#include "osc.h"

#define OWL_MIN_PERIOD  ((useconds_t) ((2.0/OWL_MAX_FREQUENCY)*1000000.0)) /* microseconsd */

typedef struct _psOSCdSystem_
{
    char *progname;
    char *phasespace;
    char *osc_server_name;
    char *configuration_filename;
    psOSCdOSC *osc_server;
    PhaseSpaceData data;
    double frequency;
    useconds_t sleep_time;
    long int  is_verbose;
    long int  is_slave;
    long int  detach;
    long int  osc_control_port;
} psOSCdSystem;

/* PROTOTYPES */

/* loop.c */
void loop();
void start_streaming();
void stop_streaming();
void quit();

/* daemon.c */
int start_daemon();

/* options.c */
int handle_options(int ac, char *av[]);

/* safe_malloc.c */
void *safe_malloc(size_t size);

/* system.c */
void system_initialize();
void system_stop();
const char *phasespace_server();
const char *osc_server_name();
psOSCdOSC *osc_server();
void set_osc_server(psOSCdOSC *os);
const char *program_name();
void set_verbose(int);
const int verbose();
const int slave();
PhaseSpaceData *data();
const int num_markers();
PhaseSpaceMarkers *markers();
const int num_rigids();
PhaseSpaceRigids *rigids();
const int osc_control_port();
char *system_info(char *);
void set_num_markers_si(psOSCdSystem *, int i);
void set_num_markers_s(psOSCdSystem *, const char *i);
void set_num_markers(int n);
void set_num_rigids_s(psOSCdSystem *, const char *i);
void set_num_rigids(int n);
void set_system_frequency_sf(psOSCdSystem *, float);
void set_system_frequency_s(psOSCdSystem *s, const char *f);
void set_system_frequency(const char *s);
float system_frequency();
useconds_t wait_period();
void wait_for_data();
void set_configuration_filename_s(psOSCdSystem *s, const char *fn);
void set_configuration_filename(const char *fn);
const char *configuration_filename();
psOSCdSystem *initialize_system(psOSCdSystem *s);
const int done();
void set_done();
const int idle();
void set_idle();
const int streaming();
void set_streaming();
const int detach();

/* errors.c */
int fatal(const char *fmt, ...);
int fatal_perror(const char *fmt, ...);
int warning(const char *fmt, ...);

/* log.c */
void _psOSCd_log(const char *, va_list);
void psOSCd_log(const char *, ...);
void psOSCd_debug(const char *, ...);
void start_logging();
void stop_logging();
void set_debug_log();

/* simulation.c */
void set_simulation_output();

/* config.c */
void configure_system(const psOSCdSystem *);

/* osc_services.c */
int start_osc_services();

/* osc_reply_to.c */
void set_connection(const char *, int, int);
void release_connection(const char *, int);
int lookup_connection(const char *, int); /* returns to_port or -1 if not found */

/* osc_query_markers.c */
void send_queried_markers(const int [], int , const char *, int);

/* signals.c */
void initialize_signals();

#endif /* !defined(_PSOSCD_H_) */
