/*
 * $Id: osc_reply_to.c 36 2010-10-04 16:40:21Z nicb $
 */

#include <stdlib.h>
#include <string.h>
#include <lo/lo.h>

#include "psOSCd.h"

typedef struct _osc_connection_
{
    char *from_address;
    int   from_port;
    int   to_port;
    struct _osc_connection_ *next;
    struct _osc_connection_ *previous;
} OSCConnection;

static OSCConnection *raw_lookup_connection(const char *, int);
static void connection_create(const char *, int, int);
static void connection_destroy(OSCConnection *);

void
set_connection(const char *from, int from_port, int to_port)
{
    OSCConnection *c = raw_lookup_connection(from, from_port);

    if (c == (OSCConnection *) NULL)
    {
        psOSCd_debug("Creating new connection with address %s to port %d\n", from, to_port);
        connection_create(from, from_port, to_port);
    }
}

void
release_connection(const char *from, int from_port)
{
    OSCConnection *c = raw_lookup_connection(from, from_port);

    if (c != (OSCConnection *) NULL)
    {
        psOSCd_debug("Destroy connection with address %s:%d\n", from, from_port);
        connection_destroy(c);
    }
    else
        psOSCd_log("Attempting to destroy non-existing connection to address %s:%d\n", from, from_port);
}

int lookup_connection(const char *host, int from_port)
{
    int result = -1;
    OSCConnection *conn = raw_lookup_connection(host, from_port);

    if (conn == (OSCConnection *) NULL)
        psOSCd_log("Connection lookup for address %s:%d failed\n", host, from_port);
    else
    {
        psOSCd_debug("Connection lookup for address %s:%d succeeded: returning port %d\n", host, from_port, conn->to_port);
        result = conn->to_port;
    }

    return result;
}

/*
 * private methods
 */

static void append_to_connection_list(OSCConnection *);
static void remove_from_connection_list(OSCConnection *);

static OSCConnection *c_first = (OSCConnection *) NULL;
static OSCConnection *c_last = (OSCConnection *) NULL;

static OSCConnection *
raw_lookup_connection(const char *host, int from_port)
{
    OSCConnection *result = (OSCConnection *) NULL;
    OSCConnection *cur = c_first;

    while (cur != (OSCConnection *) NULL)
    {
        if (!strcmp(host, cur->from_address) && from_port == cur->from_port)
        {
            result = cur;
            break;
        }
        else
            cur = cur->next;
    }

    return result;
}

static void
connection_create(const char *from, int from_port, int to_port)
{
    OSCConnection *conn = safe_malloc(sizeof(OSCConnection));

    conn->from_address = strdup(from);
    conn->from_port = from_port;
    conn->to_port = to_port;
    conn->next = conn->previous = (OSCConnection *) NULL;

    append_to_connection_list(conn);
}

static void
connection_destroy(OSCConnection *conn)
{
    remove_from_connection_list(conn);

    free(conn->from_address);
    free(conn);
}

static void
append_to_connection_list(OSCConnection *conn)
{
    if (c_last != (OSCConnection *) NULL)
    {
        c_last->next = conn;
        conn->previous = c_last;
        c_last = conn;
    }
    else /* I must be the first one */
        c_first = c_last = conn;
}

static void
remove_from_connection_list(OSCConnection *conn)
{
    OSCConnection *p = conn->previous;
    OSCConnection *n = conn->next;

    if (p != (OSCConnection *) NULL)
        p->next = n;
    else
        c_first = n;

    if (n != (OSCConnection *) NULL)
        n->previous = p;
    else
        c_last = p;

    conn->previous = conn->next = (OSCConnection *) NULL;
}
