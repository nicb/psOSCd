/*
 * $Id: osc.h 37 2010-10-04 22:27:01Z nicb $
 */

#if !defined(_psOSCd_osc_h_)
#   define _psOSCd_osc_h_

#include <lo/lo.h>

#include "owl.h"

#define OSC_PORT_BUFSIZE  (16)

typedef struct _psOSCdOSC_
{
    lo_address server;
} psOSCdOSC;

/* PROTOTYPES */

void osc_initialize();
void osc_close();
void osc_send_marker(const psOSCdOSC*, const int, const PhaseSpaceMarker *);
void osc_send_rigid(const psOSCdOSC*, const PhaseSpaceRigidDescriptor *);
psOSCdOSC *pso_new_iport(const char *, int);
void pso_destroy(psOSCdOSC *);

#endif /* !defined(_psOSCd_osc_h_) */
