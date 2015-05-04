#
# Regular cron jobs for the psoscd package
#
0 4	* * *	root	[ -x /usr/bin/psoscd_maintenance ] && /usr/bin/psoscd_maintenance
