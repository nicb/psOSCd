/*
 * $Id: osc.c 37 2010-10-04 22:27:01Z nicb $
 *
 * Ligthweight OSC API (liblo) wrapper
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "psOSCd.h"
#include "osc.h"

static void split_server_and_port(const char *, char **, char **);
static psOSCdOSC *pso_new(const char *);

void
osc_initialize()
{
    set_osc_server(pso_new(osc_server_name()));
}

void
osc_close()
{
    psOSCdOSC *osc = osc_server();
    pso_destroy(osc);
}

static psOSCdOSC *
_pso_new(const char *server, const char *port)
{
    psOSCdOSC *result = safe_malloc(sizeof(psOSCdOSC));

    result->server = lo_address_new(server, port);

    return result;
}

psOSCdOSC *
pso_new_iport(const char *server, int port)
{
    char port_s[OSC_PORT_BUFSIZE] = { '\0' };

    sprintf(port_s, "%d", port);
    return _pso_new(server, port_s);
}

static psOSCdOSC *
pso_new(const char *server_name)
{
    char *server = (char *) NULL, *port = (char *) NULL;

    split_server_and_port(server_name, &server, &port);

    return _pso_new(server, port);
}

void
pso_destroy(psOSCdOSC *osc)
{
    lo_address_free(osc->server);
    free(osc);
}

/*
 * markers low-level functions
 */

void
osc_send_marker(const psOSCdOSC *s, const int idx, const PhaseSpaceMarker *m)
{
    if (lo_send(s->server, m->tag, "ffff", m->m->x, m->m->y, m->m->z, m->m->cond) == -1)
        psOSCd_log("OSC error %d: %s\n", lo_address_errno(s->server), lo_address_errstr(s->server));
    else
        psOSCd_debug("OSC: sent marker %d (%6.2f, %6.2f, %6.2f, %6.2f) successefully\n", idx, m->m->x, m->m->y, m->m->z, m->m->cond);
}

/*
 * rigids low-level functions
 */

void
osc_send_rigid(const psOSCdOSC *s, const PhaseSpaceRigidDescriptor *r)
{
    if (lo_send(s->server, r->tag, "fffff", r->rigid->pose[0], r->rigid->pose[1], r->rigid->pose[2], r->rigid->pose[3], r->rigid->cond) == -1)
        psOSCd_log("OSC error %d: %s\n", lo_address_errno(s->server), lo_address_errstr(s->server));
    else
        psOSCd_debug("OSC: sent rigid body %d (%6.2f, %6.2f, %6.2f, %6.2f, %6.2f) successefully\n", r->tracker, r->rigid->pose[0], r->rigid->pose[1], r->rigid->pose[2], r->rigid->pose[3], r->rigid->cond);
}

/*
 * utility functions
 */

static void
split_server_and_port(const char *argstring, char **server, char **port)
{
    char *server_string = strdup(argstring);
    char *colon = strchr(server_string, ':');

    if (colon != (char *) NULL)
    {
        *port = colon + 1;
        *colon = '\0';
    }
    else
        *port = (char *) NULL;

    *server = server_string;
}
