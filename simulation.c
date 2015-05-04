/*
 * $Id: simulation.c 44 2012-08-01 23:26:55Z nicb $
 */

#include "psOSCd.h"

void set_simulation_output()
{
#if PS_SIMULATION_MODE == 1
    void set_printout_function(void (*)(const char *, ...));

    set_printout_function(psOSCd_debug);
    psOSCd_debug("Simulation output set to psOSCd_debug");
#endif /* PS_SIMULATION_MODE == 0 */
}
