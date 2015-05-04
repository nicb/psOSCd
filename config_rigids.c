/*
 * $Id: config_rigids.c 44 2012-08-01 23:26:55Z nicb $
 *
 * this is used for the configuration of rigid bodies
 */

#include <math.h>
#include <string.h>

#include "config.h"
#include "phasespace.h"

#include "psOSCd.h"

static int calculate_total_number_of_markers(config_setting_t *);

const int
rigids_handler(const config_t *c, const char *p, ConfigVar *v)
{
    /*
     * rigids configuration is in the form:
     * rigids =
     * (
     *    (
     *      { x = 0, y = 300, z = 0 },
     *      ...
     *    ),
     *    ...
     * );
     */
    PhaseSpaceRigids *psr = (PhaseSpaceRigids *) v->variable;
    config_setting_t *rigids = config_lookup(c, p);
    int nrigids = 0;

    if (rigids != (config_setting_t *) NULL)
    {
	    const int nrigids_from_conf = config_setting_length(rigids);
	    const int total_markers = calculate_total_number_of_markers(rigids);
	    int i = 0, current_marker = 0;
	
      nrigids = nrigids_from_conf;
	    psOSCd_debug("configuring %d rigid body descriptions...", nrigids);
	
	    psr->bodies = (PhaseSpaceRigidDescriptor *) safe_malloc(sizeof(PhaseSpaceRigidDescriptor) * nrigids);
	    psr->r = (OWLRigid *) safe_malloc(sizeof(OWLRigid) * nrigids);
	    psr->markers = (OWLMarker *) safe_malloc(sizeof(OWLMarker) * total_markers);
	    psr->num_rigids = nrigids;
	    psr->num_markers = total_markers;
	    psr->received = 0;
	
	    for (i = 0; i < nrigids; ++i)
	    {
	        config_setting_t *rigid = config_setting_get_elem(rigids, i);
	        const int nmarkers = config_setting_length(rigid);
	        int j = 0;
	        PhaseSpaceRigidDescriptor *psrd = psr->bodies + i;
	        psrd->num_markers = nmarkers;
	        psrd->tracker = i;
	        psrd->rigid = psr->r + i;
	        sprintf(psrd->tag, "/rb_%02d", i);
	
	        psOSCd_debug("configuring %d markers for rigid body %d", nmarkers, i);
	
	        psrd->markers = psr->markers + current_marker;
	        psrd->mcoords = (PhaseSpaceRigidMarkerCoordinates *) safe_malloc(sizeof(PhaseSpaceRigidMarkerCoordinates) * nmarkers);
	        for (j = 0; j < nmarkers; ++j)
	        {
	            config_setting_t *marker = config_setting_get_elem(rigid, j);
	            const int ncoords = config_setting_length(marker);
	            int k = 0;
	
	            if (ncoords != PHASESPACE_COORDS_PER_MARKER)
	                fatal("configuration error at line %d: wrong number of coordinates (%d != %d) for marker %d", config_setting_source_line(marker), ncoords, PHASESPACE_COORDS_PER_MARKER, j);
	
	            for (k = 0; k < ncoords; ++k)
	            {
	                config_setting_t *coord = config_setting_get_elem(marker, k);
	
	                psrd->mcoords[j].coords[k] = (float) config_setting_get_int(coord);
	                psOSCd_debug("   rigids[%d][%d][%d] = %+8.1f", i, j, k, psrd->mcoords[j].coords[k]);
	            }
	            ++current_marker;
	        }
	    }
	
	    phasespace_switch_to_rigid_tracking();
    }

    return nrigids;
}

static int
calculate_total_number_of_markers(config_setting_t *rigids)
{
    int result = 0, i = 0;
    int nrigids = config_setting_length(rigids);

    for (i = 0; i < nrigids; ++i)
    {
        config_setting_t *rigid = config_setting_get_elem(rigids, i);
        result += config_setting_length(rigid);
    }
    
    return result;
}
