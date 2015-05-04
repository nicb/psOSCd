/*
 * $Id: main.c 30 2010-05-10 02:31:58Z nicb $
 */

#include "psOSCd.h"

void initialize_phasespace_system();

int
main(int ac, char *av[])
{
    int exit_value = 0;

    initialize_phasespace_system();
    psOSCd_debug("initalize system passed");

    exit_value = handle_options(ac, av);
    set_simulation_output();
    exit_value = start_daemon();

    return exit_value;
}
