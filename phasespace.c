/*
 * $Id: phasespace.c 37 2010-10-04 22:27:01Z nicb $
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psOSCd.h"
#include "phasespace.h"

void
phasespace_connect()
{
    int init_flags = slave() ? (OWL_SLAVE | PHASESPACE_INIT_FLAGS) : PHASESPACE_INIT_FLAGS;

    if (owlInit(phasespace_server(), init_flags) < 0)
    {
        psOSCd_log("Connection to PhaseSpace Server \"%s\" failed, aborting.\n", phasespace_server());
        exit(-1);
    }
    else
        psOSCd_debug("Connection to PhaseSpace Server \"%s\" succeeded.\n", phasespace_server());
}

void
phasespace_initialize()
{
    PhaseSpaceData *d = data();

    phasespace_connect();
	
    if (!slave()) /* slave may not setup anything */
        (*d->initialize)();

    d->status = PHASESPACE_INITIALIZED;
}

void
phasespace_close()
{
    PhaseSpaceData *d = data();

    owlDone();
    (*d->destroy)();
    d->status = PHASESPACE_CLEAN;
}

void
phasespace_send_data()
{
    PhaseSpaceData *d = data();

    (*d->put)();
}

int
phasespace_load_data()
{
    PhaseSpaceData *d = data();

    return (*d->get)();
}

void
phasespace_update_frequency()
{
    owlSetFloat(OWL_FREQUENCY, system_frequency());
}

void
phasespace_start_streaming()
{
    owlSetInteger(OWL_STREAMING, OWL_ENABLE);
}

void
phasespace_stop_streaming()
{
    owlSetInteger(OWL_STREAMING, OWL_DISABLE);
}

const char *
phasespace_error(int n)
{
    static char result[PSOSCD_BUFSIZE] = { '\0' };

    if (n < 0)
        sprintf(result, "%d", n);
    else
        switch(n)
        {
            case OWL_NO_ERROR: strcpy(result, "No Error");
                               break;
            case OWL_INVALID_VALUE: strcpy(result, "Invalid Value");
                               break;
            case OWL_INVALID_ENUM: strcpy(result, "Invalid Enum");
                               break;
            case OWL_INVALID_OPERATION: strcpy(result, "Invalid Operation");
                               break;
            default: sprintf(result, "0x%x", (unsigned int) n);
                               break;
        }

    return result;
}

static void
phasespace_switch(PhaseSpaceTrackingType t, void (*init)(), void (*free)(), int (*query)(), void (*send)())
{
    PhaseSpaceData *d = data();

    if (d->status == PHASESPACE_INITIALIZED)
        phasespace_close();

    d->type = t;
    d->initialize = init;
    d->destroy = free;
    d->get = query;
    d->put = send;
}

void
phasespace_switch_to_marker_tracking()
{
    phasespace_switch(PHASESPACE_TRACKS_MARKERS, phasespace_initialize_markers,
                      phasespace_destroy_markers, phasespace_load_markers, phasespace_send_markers);
}

void
phasespace_switch_to_rigid_tracking()
{
    phasespace_switch(PHASESPACE_TRACKS_RIGIDS, phasespace_initialize_rigids,
                      phasespace_destroy_rigids, phasespace_load_rigids, phasespace_send_rigids);
}
