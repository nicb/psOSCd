/*
 * $Id: osc_services.c 42 2012-07-31 15:51:22Z nicb $
 *
 * OSC services are needed to control psOSCd via an OSC client
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <lo/lo.h>

#include "psOSCd.h"
#include "phasespace.h"

static void osc_control_error(int, const char *, const char *);
static int generic_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int quit_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int verbose_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int start_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int stop_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int connect_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int disconnect_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int frequency_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static int query_markers_handler(const char *, const char *, lo_arg **, int, lo_message, void *);
static const char *osc_pretty_print(char [], const char *, lo_arg *);

int
start_osc_services()
{
    char port[PSOSCD_BUFSIZE] = { '\0' };
    lo_server_thread st = (lo_server_thread) NULL;
    int iport = osc_control_port();
        
    psOSCd_debug("Starting OSC services on port %d...\n", iport);
    sprintf(port, "%d", iport);
    st = lo_server_thread_new(port, osc_control_error);

    lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
    lo_server_thread_add_method(st, "/quit", "", quit_handler, NULL);
    lo_server_thread_add_method(st, "/start", "", start_handler, NULL);
    lo_server_thread_add_method(st, "/stop", "", stop_handler, NULL);
    lo_server_thread_add_method(st, "/verbose", "i", verbose_handler, NULL);
    lo_server_thread_add_method(st, "/connect", "i", connect_handler, NULL);
    lo_server_thread_add_method(st, "/disconnect", NULL, disconnect_handler, NULL);
    lo_server_thread_add_method(st, "/frequency", "f", frequency_handler, NULL);
    lo_server_thread_add_method(st, "/query_markers", NULL, query_markers_handler, NULL);

    lo_server_thread_start(st);

    loop();

    return 0;
}

static void
osc_control_error(int num, const char *m, const char *path)
{
    warning("liblo server error %d in path %s: %s\n", num, path, m);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
static int
generic_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    int i;
    char buf[PSOSCD_BUFSIZE] = { '\0' };
    lo_address addr = lo_message_get_source(msg);

    psOSCd_debug("OSC message received from: address %s port %s\n", lo_address_get_hostname(addr),
                                                                    lo_address_get_port(addr));
    psOSCd_debug("OSC message received: path: <%s>\n", path);

    for (i = 0; i < argc; ++i)
	      psOSCd_debug("\targ[%d] '%c': %s\n", i, types[i], osc_pretty_print(buf, &types[i], argv[i]));

    return 1;
}

static int
quit_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    psOSCd_log("stopping the daemon\n");
    quit();

    return 0;
}

static int
start_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    psOSCd_log("start phasespace streaming\n");
    start_streaming();

    return 0;
}

static int
stop_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    psOSCd_log("stop phasespace streaming\n");
    stop_streaming();

    return 0;
}

static int
verbose_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    int result = 0;

    set_verbose(argv[0]->i);
    psOSCd_debug("setting verbosity to %d\n", verbose());

    return result;
}

static int
connect_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
    lo_address addr = lo_message_get_source(msg);
    const char *host = lo_address_get_hostname(addr);
    int port_from = atoi(lo_address_get_port(addr));
    int port_to = argv[0]->i;

    psOSCd_debug("connection request received from: address %s port %d to hook to port %d\n", host, port_from, port_to);

    set_connection(host, port_from, port_to);

    return 0;
}

static int
disconnect_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *ud)
{
    lo_address addr = lo_message_get_source(msg);
    const char *host = lo_address_get_hostname(addr);
    int port_from = atoi(lo_address_get_port(addr));

    psOSCd_debug("disconnection request received from: address %s port %d\n", host, port_from);

    release_connection(host, port_from);

    return 0;
}

static int
frequency_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *ud)
{
    psOSCdSystem *get_system();
    psOSCdSystem *s = get_system();

    set_system_frequency_sf(s, argv[0]->f);
    psOSCd_debug("setting streaming frequency to %f\n", system_frequency());

    phasespace_update_frequency();

    return 0;
}

static int
query_markers_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *ud)
{
    lo_address addr = lo_message_get_source(msg);
    const char *host = lo_address_get_hostname(addr);
    int port_from = atoi(lo_address_get_port(addr));
    int port_to = lookup_connection(host, port_from);

    psOSCd_debug("query_markers_handler(\"%s\", 0x%p, 0x%p, %d, 0x%p, 0x%p)\n", path, types, argv, argc, msg, ud);
    psOSCd_debug("attempting to reply to %s:%d\n", host, port_to);

    if (port_to > 0)
    {
        int *marker_numbers = safe_malloc(sizeof(int) * argc);
        int i = 0;

        for (i = 0; i < argc; ++i)
            marker_numbers[i] = argv[i]->i;

        psOSCd_debug("sending %d markers to %s:%d\n", argc, host, port_to);
        send_queried_markers(marker_numbers, argc, host, port_to);

       free(marker_numbers);
    }
    else
        psOSCd_log("attempted to query from an unregistered connection %s:%d\n", host, port_from);

    return 0;
}

/*
 * utility functions
 */
static void osc_pretty_print_int(char [], const char *, lo_arg *);
static void osc_pretty_print_string(char [], const char *, lo_arg *);
static void osc_pretty_print_float(char [], const char *, lo_arg *);
static void osc_pretty_print_midi(char [], const char *, lo_arg *);
static void osc_pretty_print_timetag(char [], const char *, lo_arg *);

static const char *
osc_pretty_print(char buf[], const char *type, lo_arg *arg)
{
    switch(type[0])
    {
        case 'i':
        case 'h':
                    osc_pretty_print_int(buf, type, arg);
                    break;
        case 's':
        case 'S':
                    osc_pretty_print_string(buf, type, arg);
                    break;
        case 'f':
        case 'd':
                    osc_pretty_print_float(buf, type, arg);
                    break;
        case 'c':
                    sprintf(buf, "%c", arg->c);
                    break;
        case 'm':
                    osc_pretty_print_midi(buf, type, arg);
                    break;
        case 't':
                    osc_pretty_print_timetag(buf, type, arg);
                    break;
        default:
                    sprintf(buf, "unrecognized type %s", type);
                    break;
    }

    return buf;
}

static void
osc_pretty_print_int(char buf[], const char *type, lo_arg *arg)
{
    if (!strcmp(type, "i"))
        sprintf(buf, "%d", arg->i);
    else if (!strcmp(type, "i32"))
        sprintf(buf, "%d", arg->i32);
    else if (!strcmp(type, "h"))
        sprintf(buf, "%lld", (long long int) arg->h);
    else if (!strcmp(type, "i64"))
        sprintf(buf, "%lld", (long long int) arg->i64);
}

static void
osc_pretty_print_string(char buf[], const char *type, lo_arg *arg)
{
    if (!strcmp(type, "s"))
        sprintf(buf, "%s", &arg->s);
    else if (!strcmp(type, "S"))
        sprintf(buf, "%s", &arg->S);
}

static void
osc_pretty_print_float(char buf[], const char *type, lo_arg *arg)
{
    if (!strcmp(type, "f"))
        sprintf(buf, "%g", arg->f);
    else if (!strcmp(type, "f32"))
        sprintf(buf, "%g", arg->f32);
    else if (!strcmp(type, "h"))
        sprintf(buf, "%g", arg->d);
    else if (!strcmp(type, "f64"))
        sprintf(buf, "%g", arg->f64);
}

static void
osc_pretty_print_midi(char buf[], const char *type, lo_arg *arg)
{
    if (type[0] == 'm')
        sprintf(buf, "midi: 0x%x 0x%x 0x%x 0x%x", arg->m[0], arg->m[1], arg->m[2], arg->m[3]);
}

static void
osc_pretty_print_timetag(char buf[], const char *type, lo_arg *arg)
{
    if (type[0] == 't')
        sprintf(buf, "OSC timetag: sec: %u frac: %u", arg->t.sec, arg->t.frac); 
}
