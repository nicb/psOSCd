/*
 * $Id: options.c 39 2010-11-02 23:42:57Z nicb $
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "psOSCd.h"

static const char *ack_shortopts = "p:o:m:r:vsf:c:C:n";
static const struct option ack_options[] =
{
    { "phasespace_server", required_argument, NULL, 'p' },
    { "osc_server", required_argument, NULL, 'o' },
    { "num_markers", required_argument, NULL, 'm' },
    { "num_rigids", required_argument, NULL, 'r' },
    { "verbose", no_argument, NULL, 'v' },
    { "slave", no_argument, NULL, 's' },
    { "frequency", required_argument, NULL, 'f' },
    { "config", required_argument, NULL, 'c' },
    { "osc_control_port", required_argument, NULL, 'C' },
    { "no_detach", no_argument, NULL, 'n' },
    { (char *) NULL, 0, NULL, '.' },
};

static void usage(const char *pn, const struct option *opts);

int
handle_options(int ac, char *av[])
{
    int result = 0, idx = 0, retval = -1;
    psOSCdSystem temp_config = { '\0' };
    psOSCdSystem *get_system();
    psOSCdSystem *s = get_system();
    char buffer[PSOSCD_BUFSIZE];
    
    initialize_system(&temp_config);
    temp_config.progname = s->progname = strdup(av[0]);
    while((retval = getopt_long(ac, av, ack_shortopts, ack_options, &idx)) >= 0)
    {
        switch(retval)
        {
            case 'p': temp_config.phasespace = strdup(optarg);
                      break;
            case 'o': temp_config.osc_server_name = strdup(optarg);
                      break;
            case 'm': set_num_markers_s(&temp_config, optarg);
                      break;
            case 'r': set_num_rigids_s(&temp_config, optarg);
                      break;
            case 'v': temp_config.is_verbose = 1;
	              break;
            case 'f': set_system_frequency_s(&temp_config, optarg);
	              break;
            case 'c': set_configuration_filename_s(&temp_config, optarg);
	              break;
            case 'C': temp_config.osc_control_port = atoi(optarg);
	              break;
            case 'n': temp_config.detach = 0;
                break;
            case 's': temp_config.is_slave = 1;
            case  0 : break;
            default:
            case '?': usage(program_name(), ack_options);
                      result = -1;
                      break;
        }
    }

    configure_system(&temp_config);

    /* 
     * FIXME: this check might be moved somewhere else, since we will be able
     * to start without knowing which servers we are going to address
     */
    if (phasespace_server() == (char *) NULL || osc_server_name() == (char *) NULL)
    {
        usage(program_name(), ack_options);
        result = fatal("Phasespace or OSC server (or both) missing, aborting.\n");
    }

    psOSCd_debug("handle_options() exiting with result = %d and the following values: %s\n", result, system_info(buffer));

    return result;
}

static void
usage(const char *prog_name, const struct option *options)
{
    char buffer[8192] = { '\0' };
    const struct option *cur_opt = options;

    sprintf(buffer, "Usage: %s", prog_name);

    do
    {
        char *p = buffer + strlen(buffer);
        const char *fmt = cur_opt->has_arg == no_argument ? "[-%c | --%s]" : "[[-%c | --%s] name]";
        *p++ = ' ';
        sprintf(p, fmt, cur_opt->val, cur_opt->name);
        ++cur_opt;
    } while (cur_opt->name != (char *) NULL);

    buffer[strlen(buffer)] = '\n';

    warning(buffer);
}
