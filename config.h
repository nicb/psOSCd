/*
 * $Id: config.h 36 2010-10-04 16:40:21Z nicb $
 */

#if !defined(_config_h_)
#   define _config_h_

#include <libconfig.h>

/*
 * TODO: here we will have to include some LIBCONFIG version macro to set up
 * an error when the library included is too old (we will be able to do this
 * only after switching to libconfig8 1.4.xx)
 */

struct _config_var_;

typedef int (*set_function_ptr)(const config_t *, const char *, struct _config_var_ *);

typedef struct _config_var_
{
    const char *path;
    void *variable;
    set_function_ptr handler;
} ConfigVar;

#endif /* !defined(_config_h_) */
