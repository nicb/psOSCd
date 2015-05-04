/*
 * $Id: loop.c 39 2010-11-02 23:42:57Z nicb $
 */

#include "psOSCd.h"
#include "phasespace.h"
#include "osc.h"

void
loop(void)
{
	system_initialize();
	
	/* 
	* FIXME: this is just a stub to recover basic backwards functionality
	* It will be removed in future versions, where streaming will be
	* started/stopped by control clients.
	* Please check [ticket:21 #21] to see how it will develop
	*/
	start_streaming();
	
	while (!done())
	{
		int n = phasespace_load_data();
		
		psOSCd_debug("Got data from %d objects\n", n);
		
		if (n == 0)
		    continue;
		
		if (n < 0)
    {
      set_done();
      break;
    }
		
		if (n > 0)
		    phasespace_send_data();

    psOSCd_debug("Returning to top of the loop, done() returns: %d\n", done());
  }

    psOSCd_debug("Closing the psOSCd system...\n");
    system_stop();
}

void
start_streaming()
{
    set_streaming();
}

void
stop_streaming()
{
    set_idle();
}

void
quit()
{
    set_done();
}
