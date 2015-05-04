/*
 * $Id: system.c 44 2012-08-01 23:26:55Z nicb $
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <owl.h>

#include "psOSCd.h"

static const psOSCdSystem fallback_psOSCd_system =
{
    (char *) NULL,
    (char *) NULL,
    (char *) NULL,
    (char *) NULL,
    (psOSCdOSC *) NULL,
    {
        {{ '\0' }},
        PHASESPACE_TRACKS_MARKERS,
        PHASESPACE_CLEAN,
        PHASESPACE_IDLE,
        phasespace_initialize_markers,
        phasespace_destroy_markers,
        phasespace_load_markers,
        phasespace_send_markers,
    },
    OWL_MAX_FREQUENCY, /* default frequency */
    OWL_MIN_PERIOD, /* default period */
    0, /*  default verbose level */
    0, /* default slave behaviour */
    1, /* default detach (yes) */
    4599 /* default osc control port */
};

static const psOSCdSystem empty_psOSCd_system =
{
    (char *) NULL,
    (char *) NULL,
    (char *) NULL,
    (char *) NULL,
    (psOSCdOSC *) NULL,
    {
        {{ (PhaseSpaceMarker *) NULL, -1, -1, (OWLMarker *) NULL }},
        PHASESPACE_TRACKS_MARKERS,
        PHASESPACE_CLEAN,
        PHASESPACE_IDLE,
        phasespace_initialize_markers,
        phasespace_destroy_markers,
        phasespace_load_markers,
        phasespace_send_markers
    },
    -1.0,
    -1,
    -1,
    -1,
    -1,
    -1
};

static psOSCdSystem psOSCd_system;

/*
 * private prototypes
 */

psOSCdSystem *get_system(); /* system.c */

void
system_initialize()
{
    osc_initialize();
    phasespace_initialize();
}

void
system_stop()
{
    phasespace_close();
    osc_close();
}

const char *
phasespace_server()
{
    return psOSCd_system.phasespace;
}

const char *
osc_server_name()
{
    return psOSCd_system.osc_server_name;
}

psOSCdOSC *
osc_server()
{
    return psOSCd_system.osc_server;
}

void
set_osc_server(psOSCdOSC *s)
{
    psOSCd_system.osc_server = s;
}

const char *
program_name()
{
    return psOSCd_system.progname;
}

void
set_verbose(int v)
{
    psOSCd_system.is_verbose = v == 0 ? v : 1;
    if (verbose())
        set_debug_log();
}

const int
verbose()
{
    return psOSCd_system.is_verbose ? 1 : 0;
}

const int
done()
{
    return psOSCd_system.data.mode == PHASESPACE_DONE ? 1 : 0;
}

void
set_done()
{
    psOSCd_system.data.mode = PHASESPACE_DONE;
}

void
set_idle()
{
    phasespace_stop_streaming();
    psOSCd_system.data.mode = PHASESPACE_IDLE;
}

const int
streaming()
{
    return psOSCd_system.data.mode == PHASESPACE_STREAMING ? 1 : 0;
}

void
set_streaming()
{
    psOSCd_system.data.mode = PHASESPACE_STREAMING;
}

void
reset_streaming()
{
    set_idle();
}

const int
slave()
{
    return psOSCd_system.is_slave ? 1 : 0;
}

PhaseSpaceData *
data()
{
    return &psOSCd_system.data;
}

const int
num_markers()
{
    return (data()->type == PHASESPACE_TRACKS_MARKERS) ? data()->u.markers.num_markers : 0;
}

const int
num_rigids()
{
    return (data()->type == PHASESPACE_TRACKS_RIGIDS) ? data()->u.rigids.num_rigids : 0;
}

char *
system_info(char *buffer)
{
    sprintf(buffer, "phasespace_server = %s, OSC_server = %s, phasespace_markers = %d, phasespace_rigids = %d, verbose flag = %d, slave flag = %d, detach flag = %d, system frequency = %6.1f, sleep_period = %ld", phasespace_server(), osc_server_name(), num_markers(), num_rigids(), verbose(), slave(), detach(), system_frequency(), (long int) wait_period());
    return buffer;
}

void
set_system_frequency_sf(psOSCdSystem *s, float f)
{
    float frq = f > OWL_MAX_FREQUENCY ? OWL_MAX_FREQUENCY : f;
    s->frequency = frq;
    s->sleep_time = ((useconds_t)((2.0/s->frequency)*1000000.0)); /* microseconds */
    psOSCd_debug("set_system_frequency_sf(s = %x, f = %f) -> s->frequency = %6.1f Hz, s->sleep_time = %ld usecs\n", s->frequency, s->sleep_time);
}

void
set_system_frequency_s(psOSCdSystem *s, const char *arg)
{
    float frq = atof(arg);
    set_system_frequency_sf(s, frq);
}

void
set_system_frequency(const char *arg)
{
    set_system_frequency_s(get_system(), arg);
}

float
system_frequency()
{
    return (float) psOSCd_system.frequency;
}

useconds_t
wait_period()
{
    return psOSCd_system.sleep_time;
}

void
wait_for_data()
{
    psOSCd_debug("usleep(%ld)\n", wait_period());
    usleep(wait_period());
}

void
set_num_markers_si(psOSCdSystem *s, int n)
{
    phasespace_switch_to_marker_tracking();
    s->data.u.markers.num_markers = n;
}

void
set_num_markers_s(psOSCdSystem *s, const char *sn)
{
    set_num_markers_si(s, atoi(sn));
}

void
set_num_markers(int n)
{
    set_num_markers_si(&psOSCd_system, n);
}

PhaseSpaceMarkers *
markers()
{
    PhaseSpaceData *d = data();

    return (d->type == PHASESPACE_TRACKS_MARKERS) ? &d->u.markers : (PhaseSpaceMarkers *) NULL;
}

static void
set_num_rigids_si(psOSCdSystem *s, int n)
{
    phasespace_switch_to_rigid_tracking();
    s->data.u.rigids.num_rigids = n;
}

void
set_num_rigids_s(psOSCdSystem *s, const char *sn)
{
    set_num_rigids_si(s, atoi(sn));
}

void
set_num_rigids(int n)
{
    set_num_rigids_si(&psOSCd_system, n);
}

PhaseSpaceRigids *
rigids()
{
    PhaseSpaceData *d = data();

    return (d->type == PHASESPACE_TRACKS_RIGIDS) ? &d->u.rigids : (PhaseSpaceRigids *) NULL;
}

void
set_configuration_filename_s(psOSCdSystem *s, const char *arg)
{
    s->configuration_filename = strdup(arg);
}

void
set_configuration_filename(const char *arg)
{
    set_configuration_filename_s(get_system(), arg);
}

const int
detach()
{
    return (int) psOSCd_system.detach;
}

const char *
configuration_filename()
{
    return psOSCd_system.configuration_filename;
}

const int
osc_control_port()
{
    return (int) psOSCd_system.osc_control_port;
}


psOSCdSystem *
initialize_system(psOSCdSystem *s)
{
    memcpy(s, &empty_psOSCd_system, sizeof(psOSCdSystem));
    return s;
}

void
initialize_phasespace_system()
{
    memcpy(&psOSCd_system, &fallback_psOSCd_system, sizeof(psOSCdSystem));
}

psOSCdSystem *
get_system()
{
    return &psOSCd_system;
}
