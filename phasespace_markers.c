/*
 * $Id: phasespace_markers.c 37 2010-10-04 22:27:01Z nicb $
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psOSCd.h"
#include "phasespace.h"

static void markers_new(int m);
static void phasespace_markers_master_init();

void
phasespace_initialize_markers()
{
    phasespace_markers_master_init();
}

static void
phasespace_markers_master_init()
{
    int tracker = 0, i = 0;
    PhaseSpaceMarkers *psm = markers(); /* caution: they are half-initialized here */
    int num_markers = psm->num_markers; /* though these should aready be set by now */

    /* initialize data structures */
    markers_new(num_markers);

    owlTrackeri(tracker, OWL_CREATE, OWL_POINT_TRACKER);

    for (i = 0; i < num_markers; ++i)
        owlMarkeri(MARKER(tracker, i), OWL_SET_LED, i);

    owlTracker(tracker, OWL_ENABLE);

    if(!owlGetStatus())
    {
        psOSCd_log("error in point tracker setup: %s\n", phasespace_error(owlGetError()));
        exit(-2);
    }

    owlSetFloat(OWL_FREQUENCY, system_frequency());
}

/*
 * function provided by Kan Anant, Phasespace Inc.
 * April 15, 2010
 */

static int
get_markers(PhaseSpaceMarkers *psm)
{
    int n = 0, ret = 0;
    
    /* request data */
    phasespace_start_streaming();

    /* read data */
    while(n == 0 && ret >= 0)
      while((ret = owlGetMarkers(psm->owl_markers, psm->num_markers)) > 0)
        n = ret;

    /* stop data */
    phasespace_stop_streaming();

    return n;
}

int
phasespace_load_markers()
{
    int err = OWL_NO_ERROR;
    PhaseSpaceMarkers *psm = markers();
    psm->received = get_markers(psm);

    if((err = owlGetError()) != OWL_NO_ERROR) 
    {
        psOSCd_log("PhaseSpace Error: %s\n", phasespace_error(err));
        psm->received = 0;
    }

    return psm->received;
}

static void
markers_new(int num_markers)
{
    int i = 0;
    PhaseSpaceMarkers *psm = markers();

    psm->markers = (PhaseSpaceMarker *) safe_malloc(sizeof(PhaseSpaceMarker)*num_markers);
    psm->owl_markers = (OWLMarker *) safe_malloc(sizeof(OWLMarker)*num_markers);

    for(i = 0; i < num_markers; ++i)
    {
        psm->markers[i].m = &psm->owl_markers[i];
        sprintf(psm->markers[i].tag, "/m_%02d", i);
    }

    psOSCd_debug("psd_new: allocated PhaseSpaceMarker 0x%p (sizeof(PhaseSpaceMarker) = %d * num_markers = %d)\n", psm->markers, sizeof(PhaseSpaceMarker), num_markers);

    psm->num_markers = num_markers;
    psm->received = 0;
}

void
phasespace_destroy_markers()
{
    PhaseSpaceMarkers *psm = markers();

    free(psm->owl_markers);
    free(psm->markers);
}

static void phasespace_osc_send_marker(const int, const PhaseSpaceMarker *m);

void
phasespace_send_markers()
{
    PhaseSpaceMarkers *psm = markers();
    int i = 0;

    for (i = 0; i < psm->received; ++i)
    {
        PhaseSpaceMarker *m = psm->markers + i;
        if (m->m->cond > 0.0)
        {
            if (streaming())
            {
                psOSCd_debug("streaming marker %d: x = %6.2f, y = %6.2f, z = %6.2f\n", i, m->m->x, m->m->y, m->m->z);
                phasespace_osc_send_marker(i, m);
            }
            else
                psOSCd_debug("NOT streaming marker %d: x = %6.2f, y = %6.2f, z = %6.2f (streaming = %d)\n", i, m->m->x, m->m->y, m->m->z, streaming);
        }
        else
            psOSCd_debug("marker %d: cond = %6.2f\n", i, m->m->cond);
    }
}

static void
phasespace_osc_send_marker(const int i, const PhaseSpaceMarker *m)
{
    psOSCdOSC *osc = osc_server();

    osc_send_marker(osc, i, m);
}
