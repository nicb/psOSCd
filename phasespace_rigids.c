/*
 * $Id: phasespace_rigids.c 39 2010-11-02 23:42:57Z nicb $
 */

#include <stdlib.h>

#include "psOSCd.h"
#include "phasespace.h"

static void phasespace_rigids_master_init();
static void phasespace_osc_send_rigid(const PhaseSpaceRigidDescriptor *psrd);

void
phasespace_initialize_rigids()
{
    phasespace_rigids_master_init();
}

static void
phasespace_rigids_master_init()
{
    PhaseSpaceRigids *psrs = rigids();
    int i = 0;

    for (i = 0; i < psrs->num_rigids; ++i)
    {
        PhaseSpaceRigidDescriptor *psrd = &psrs->bodies[i];
        int tracker = psrd->tracker, j = 0;

        owlTrackeri(tracker, OWL_CREATE, OWL_RIGID_TRACKER);

        for (j = 0; j < psrd->num_markers; ++j)
        {
            owlMarkeri(MARKER(tracker, j), OWL_SET_LED, j);
            owlMarkerfv(MARKER(tracker, j), OWL_SET_POSITION, psrd->mcoords[j].coords);
        }

        owlTracker(tracker, OWL_ENABLE);

        if(!owlGetStatus())
            fatal("error in rigid body setup n%d: %s\n", i, phasespace_error(owlGetError()));
    }

    owlSetFloat(OWL_FREQUENCY, system_frequency());
}

/*
 * cannibalized from the get_markers function provided by Kan Anant, Phasespace Inc.
 * April 15, 2010
 */

static int
get_rigids(OWLRigid *rigids, size_t rcount, OWLMarker *markers, size_t mcount)
{
    int n = 0, ret = 0, m = 0;
  
    /* request data */
    psOSCd_debug("calling phasespace_start_streaming()\n");
    phasespace_start_streaming();
    psOSCd_debug("after calling phasespace_start_streaming()\n");

    /* read data */
    while(n == 0 && ret >= 0)
    {
        psOSCd_debug("calling owlGetRigids(%x, %d)\n", rigids, rcount);
        while ((ret = owlGetRigids(rigids, rcount)) > 0)
        {
            n = ret;
            psOSCd_debug("%d = owlGetRigids(%x, %d)\n", ret, rigids, rcount);
		        /*
		         * as the manual says: markers have to be read, even if they are not used
		         */
            psOSCd_debug("calling owlGetMarkers(%x, %d)\n", markers, mcount);
		        m = owlGetMarkers(markers, mcount);
            psOSCd_debug("%d = owlGetMarkers(%x, %d)\n", m, markers, mcount);
        }
        wait_for_data();
    }

    /* stop data */
    psOSCd_debug("calling phasespace_stop_streaming()\n");
    phasespace_stop_streaming();
    psOSCd_debug("after calling phasespace_stop_streaming()\n");

    return n;
}

int
phasespace_load_rigids()
{
    int err = OWL_NO_ERROR;
    PhaseSpaceRigids *psr = rigids();
    psr->received = get_rigids(psr->r, psr->num_rigids, psr->markers, psr->num_markers);

    if((err = owlGetError()) != OWL_NO_ERROR) 
    {
        psOSCd_log("PhaseSpace Error: %s\n", phasespace_error(err));
        psr->received = 0;
    }

    return psr->received;
}

void
phasespace_send_rigids()
{
    PhaseSpaceRigids *psr = rigids();
    int i = 0;

    for (i = 0; i < psr->received; ++i)
    {
        PhaseSpaceRigidDescriptor *r = psr->bodies + i;
        if (r->rigid->cond > 0.0)
        {
            if (streaming())
            {
                psOSCd_debug("streaming rigid body %d: x = %6.2f, y = %6.2f, z = %6.2f, pose = %6.2f\n", r->tracker, r->rigid->pose[0], r->rigid->pose[1], r->rigid->pose[2], r->rigid->pose[3]);
                phasespace_osc_send_rigid(r);
            }
            else
                psOSCd_debug("NOT streaming rigid body %d: x = %6.2f, y = %6.2f, z = %6.2f, pose = %6.2f\n", r->tracker, r->rigid->pose[0], r->rigid->pose[1], r->rigid->pose[2], r->rigid->pose[3]);
        }
        else
            psOSCd_debug("rigid body %d: cond = %6.2f\n", r->tracker, r->rigid->cond);
    }
}

void
phasespace_destroy_rigids()
{
    int i = 0;
    PhaseSpaceRigids *psr = rigids();

    for (i = 0; i < psr->num_rigids; ++i)
    {
        PhaseSpaceRigidDescriptor *psrd = psr->bodies + i;
        free(psrd->mcoords);
    }

    free(psr->markers);
    free(psr->r);
    free(psr->bodies);
}

static void
phasespace_osc_send_rigid(const PhaseSpaceRigidDescriptor *psrd)
{
    psOSCdOSC *osc = osc_server();

    osc_send_rigid(osc, psrd);
}
