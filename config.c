/*
 * $Id: config.c 43 2012-08-01 21:15:32Z nicb $
 *
 * The configuration file for psOSCd is modeled over libconfig8, and uses that
 * library and API
 */

#include <sys/stat.h>
#include <string.h>

#include "config.h"
#include "psOSCd.h"

/*
 * private prototypes
 */
static void load_configuration_from_file(psOSCdSystem *s);
static void override_with_command_line_options(psOSCdSystem *dest, const psOSCdSystem *src);
psOSCdSystem *get_system(); /* system.c */

/*
 * configure_system:
 * - first load and parse configuration file
 * - load system configuration with config file data
 * - then load option variables if present (override configuration variables on file)
 */
void
configure_system(const psOSCdSystem *temp)
{
    psOSCdSystem *sys = get_system();

    sys->configuration_filename = temp->configuration_filename;
    load_configuration_from_file(sys);
    override_with_command_line_options(sys, temp);
}

/*
 * private configuration functions
 */
static const char * get_configuration_filename_path(psOSCdSystem *);

static const char *default_configuration_filename = PSOSCD_DEFAULT_CONFIGURATION_FILENAME;

static int configuration_file_exists(const char *);
static int string_handler(const config_t *, const char *, ConfigVar *);
static int int_handler(const config_t *, const char *, ConfigVar *);
/* static int float_handler(const config_t *, const char *, ConfigVar *); */
static int num_markers_handler(const config_t *, const char *, ConfigVar *);
static int system_frequency_handler(const config_t *, const char *, ConfigVar *);
       int rigids_handler(const config_t *, const char *, ConfigVar *); /* config_rigids.c */

static void
load_configuration_from_file(psOSCdSystem *s)
{
    const char *cfname = get_configuration_filename_path(s);

    if (configuration_file_exists(cfname))
    {
	    config_t config_settings;
	    int i = 0;
      /*
       * NOTE: the order in which configuration variables get evaluated is
       * extremely significant for proper initialization. Do not tamper with
       * it unless you know what you are doing.
       */
	    ConfigVar config_vars[] =
	    {
	        { "verbose", &s->is_verbose, int_handler },
	        { "slave", &s->is_slave, int_handler },
	        { "phasespace_server", &s->phasespace, string_handler },
	        { "osc_server", &s->osc_server_name, string_handler },
	        { "frequency", s, system_frequency_handler },
	        { "num_markers", &s->data.u.markers.num_markers, num_markers_handler },
	        { "rigids", &s->data.u.rigids, rigids_handler },
	    };
	    const int num_elems = sizeof(config_vars)/sizeof(ConfigVar); /* num_elems = number of elements in config vars */
      int res = CONFIG_TRUE;
	
	    config_init(&config_settings);
	
	    if ((res = config_read_file(&config_settings, cfname)) == CONFIG_FALSE)
	        fatal("configuration - line %d: %s (configuration error)", config_error_line(&config_settings),
	                                                                   config_error_text(&config_settings));
	
	    for (i = 0; i < num_elems; ++i)
      {
	        res = (*(config_vars[i].handler))(&config_settings, config_vars[i].path, &config_vars[i]);
          if (res == CONFIG_FALSE)
          {
              if (config_error_text(&config_settings) != (const char *) NULL)
	              fatal("configuration - line %d: %s (configuration error)", config_error_line(&config_settings),
	                                                                         config_error_text(&config_settings));
              else
                psOSCd_debug("configuration: missing variable %s", config_vars[i].path);
          }
      }
	
	    config_destroy(&config_settings);
    }
}

static int
configuration_file_exists(const char *fname)
{
    struct stat tmpbuf;
    int status = stat(fname, &tmpbuf);
    int result = status ? 0 : 1; /* must invert result: 0 => true, !0 => false */

    if (status < 0)
        fatal_perror(fname);

    return result;
}

static const char *
get_configuration_filename_path(psOSCdSystem *s)
{
    return s->configuration_filename != (char *) NULL ? s->configuration_filename : default_configuration_filename;
}

static void
override_with_command_line_options(psOSCdSystem *dest, const psOSCdSystem *src)
{
    if (src->phasespace != (char *) NULL)
        dest->phasespace = src->phasespace;
    if (src->osc_server_name != (char *) NULL)
        dest->osc_server_name = src->osc_server_name;
    if (src->frequency != -1.0)
        set_system_frequency_sf(dest, src->frequency);
    if (src->data.u.markers.num_markers != -1)
        set_num_markers_si(dest, src->data.u.markers.num_markers);
    if (src->is_verbose != -1)
        dest->is_verbose = src->is_verbose;
    if (src->is_slave != -1)
        dest->is_slave = src->is_slave;
    if (src->detach != -1)
        dest->detach = src->detach;
}

static int
string_handler(const config_t *c, const char *p, ConfigVar *v)
{
    config_setting_t *cs = config_lookup(c, p);
    int result = cs == (config_setting_t *) NULL ? CONFIG_FALSE : CONFIG_TRUE;
        
    if (result == CONFIG_TRUE)
       *((char **) v->variable) = strdup(config_setting_get_string(cs));

    return result;
}

/* this is not being used right now, so we want to shut up the compiler and we
 * comment it out
 */

/*
static int
float_handler(const config_t *c, const char *p, ConfigVar *v)
{
    return config_lookup_float(c, p, (double *) v->variable);
}
*/


static int
int_handler(const config_t *c, const char *p, ConfigVar *v)
{
    return config_lookup_int(c, p, (long int *) v->variable);
}

static int
num_markers_handler(const config_t *c, const char *p, ConfigVar *v)
{
    int result = int_handler(c, p, v);
    phasespace_switch_to_marker_tracking();
    return result;
}

static int
system_frequency_handler(const config_t *c, const char *p, ConfigVar *v)
{
    double value = 0.0;
    int result =  config_lookup_float(c, p, (double *) &value);
    set_system_frequency_sf((psOSCdSystem *) v->variable, value);
    return result;
}
