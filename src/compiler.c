#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

static pthread_mutex_t global_sync_lock = PTHREAD_MUTEX_INITIALIZER;

uint32_t __sync_val_compare_and_swap_4(uint32_t *ptr, uint32_t old,
		uint32_t val)
{
	sigset_t oset, set;

	sigfillset(&set);
	pthread_sigmask(SIG_SETMASK, &set, &oset);
	pthread_mutex_lock(&global_sync_lock);
	if (*ptr == old)
		*ptr = val;
	else
		old = *ptr;
	pthread_mutex_unlock(&global_sync_lock);
	pthread_sigmask(SIG_SETMASK, &oset, NULL);

	return old;
}
