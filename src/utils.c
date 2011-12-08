#include <stdio.h>
#include "utils.h"

void
tmp_thread_enter ()
{
	return;
	clutter_threads_enter ();
}

void
tmp_thread_leave ()
{
	return;
	clutter_threads_leave ();
}
