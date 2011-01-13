#include "detach.h"
void detach(void)
{
	int x;
	int pgrp;

	chdir("/");

	if ((x = fork()) > 0)
	    exit(0); //exit parent, so we have the child
	else if (x == -1) {
	    perror("fork");
	    fprintf(stderr, "unable to fork new process\n");
	    exit(1);
	}
	
	raise(SIGSTOP);
/* setsid - run a program in a new session*/
	if ((pgrp = setsid()) == -1) {
	    perror("setsid");
	    fprintf(stderr, "setsid failed\n");
	    exit(1);
	}

	/* close out the standard file descriptors */
	if (freopen("/dev/null", "r", stdin) == NULL) {
	    fprintf(stderr, "unable to replace stdin with /dev/null:\n");
	    /* continue anyhow -- note we can't close out descriptor 0 because we
	     * have nothing to replace it with, and if we didn't have a descriptor
	     * 0 the next file would be created with that value ... leading to
	     * havoc.
	     */
	}
	if (freopen("/dev/null", "w", stdout) == NULL) {
	    fprintf(stderr, "unable to replace stdout with /dev/null: \n");
	}
	/* stderr is a tricky one, we really want it to be the error_log,
	 * but we haven't opened that yet.  So leave it alone for now and it'll
	 * be reopened moments later.
	 */
}
