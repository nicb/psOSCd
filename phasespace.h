/*
 * $Id: phasespace.h 37 2010-10-04 22:27:01Z nicb $
 */

#if !defined(_PHASESPACE_H_)
#   define _PHASESPACE_H_

#include "owl.h"

#if !defined(DEFAULT_PHASESPACE_MARKERS)
#   define DEFAULT_PHASESPACE_MARKERS (32)
#endif /* !defined(DEFAULT_PHASESPACE_MARKERS) */

#if !defined(PHASESPACE_INIT_FLAGS)
#   define PHASESPACE_INIT_FLAGS (0)
#endif /* !defined(PHASESPACE_INIT_FLAGS */

#define PHASESPACE_COORDS_PER_MARKER  (3)
#define PHASESPACE_TAG_SIZE  (6)

typedef struct _PhaseSpaceMarker_
{
    OWLMarker *m;
    char tag[PHASESPACE_TAG_SIZE];
} PhaseSpaceMarker;

typedef struct _PhaseSpaceMarkers_
{
    PhaseSpaceMarker *markers;
    int num_markers;
    int received;
    OWLMarker *owl_markers;
} PhaseSpaceMarkers;

typedef struct _PhaseSpaceRigidMarkerCoordinates_
{
    float coords[PHASESPACE_COORDS_PER_MARKER];
} PhaseSpaceRigidMarkerCoordinates;

typedef struct _PhaseSpaceRigidDescriptor_
{
    int num_markers;
    int tracker;                        /* we seem to need a tracker for each rigid body */
	  OWLRigid *rigid;                 /* connected to the OWLRigid array in the container */
    OWLMarker *markers;             /* connected to the OWLMarker array in the container */
    PhaseSpaceRigidMarkerCoordinates *mcoords;/* n marker coordinates for each rigid body */
    char tag[PHASESPACE_TAG_SIZE]; 
} PhaseSpaceRigidDescriptor;

typedef struct _PhaseSpaceRigids_
{
    PhaseSpaceRigidDescriptor *bodies;
    int num_rigids;
    int received;
    int num_markers;                                          /* total number of markers */
    OWLRigid *r;
    OWLMarker *markers;
} PhaseSpaceRigids;

typedef enum _PhaseSpaceStatus_
{
    PHASESPACE_CLEAN,
    PHASESPACE_INITIALIZED
} PhaseSpaceStatus;

typedef enum _PhaseSpaceMode_
{
    PHASESPACE_IDLE,
    PHASESPACE_STREAMING,
    PHASESPACE_DONE
} PhaseSpaceMode;

typedef enum _PhaseSpaceTrackingType_
{
    PHASESPACE_TRACKS_MARKERS,
    PHASESPACE_TRACKS_RIGIDS
} PhaseSpaceTrackingType;

typedef union _PhaseSpaceInnerData_
{
    PhaseSpaceMarkers markers;
    PhaseSpaceRigids  rigids;
} PhaseSpaceInnerData;

typedef struct _PhaseSpaceData_
{
    PhaseSpaceInnerData u;
    PhaseSpaceTrackingType type;
    PhaseSpaceStatus status;
    PhaseSpaceMode mode;
    void (*initialize)(void);
    void (*destroy)(void);
    int (*get)(void);
    void (*put)(void);
} PhaseSpaceData;


/* COMMON PROTOTYPES */

void phasespace_connect();
void phasespace_initialize();
int  phasespace_load_data();
void phasespace_send_data();
void phasespace_close();
void phasespace_update_frequency();
const char *phasespace_error(int n);
void phasespace_switch_to_marker_tracking();
void phasespace_switch_to_rigid_tracking();
void phasespace_start_streaming();
void phasespace_stop_streaming();

/* MARKERS PROTOTYPES */

void phasespace_initialize_markers();
int  phasespace_load_markers();
void phasespace_send_markers();
void phasespace_destroy_markers();

/* RIGID PROTOTYPES */

void phasespace_initialize_rigids();
int  phasespace_load_rigids();
void phasespace_send_rigids();
void phasespace_destroy_rigids();


#endif /* !defined(_PHASESPACE_H_) */
