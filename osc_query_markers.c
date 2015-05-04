/*
 * $Id: osc_query_markers.c 35 2010-10-04 09:47:45Z nicb $
 */

#include <lo/lo.h>

#include "psOSCd.h"
#include "osc.h"

void
send_queried_markers(const int marker_numbers[], int ac, const char *host, int port_to)
{
    PhaseSpaceMarkers *psm = markers();
    int i = 0;

    for (i = 0; i < ac; ++i)
    {
        int mn = i;

        if (mn < psm->num_markers)
        {
            PhaseSpaceMarker *m = psm->markers + i;
            psOSCdOSC *osc_serv = pso_new_iport(host, port_to);
           
            psOSCd_debug("marker %d: x = %6.2f, y = %6.2f, z = %6.2f, cond = %6.2f\n", i, m->m->x, m->m->y, m->m->z, m->m->cond);
            osc_send_marker(osc_serv, i, m);
            pso_destroy(osc_serv);
        }
    }
}
