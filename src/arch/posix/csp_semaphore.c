#include "../../csp_semaphore.h"

#include <inttypes.h>
#include <csp/csp.h>
#include <csp/csp_debug.h>

#ifdef __APPLE__

/* macOS: see csp_semaphore.h for why this doesn't use sem_t directly. */

void csp_bin_sem_init(csp_bin_sem_t * sem) {
	sem->sem = dispatch_semaphore_create(1);
	atomic_init(&sem->available, 1);
}

int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout) {

	dispatch_time_t when;
	if (timeout == CSP_MAX_TIMEOUT) {
		when = DISPATCH_TIME_FOREVER;
	} else {
		when = dispatch_time(DISPATCH_TIME_NOW, (int64_t) timeout * 1000000LL /* ms -> ns */);
	}

	if (dispatch_semaphore_wait(sem->sem, when) != 0) {
		return CSP_SEMAPHORE_ERROR;
	}

	atomic_store(&sem->available, 0);
	return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_post(csp_bin_sem_t * sem) {

	/* Only actually signal if we're the one flipping 0 -> 1, so repeated
	 * posts don't push the underlying dispatch semaphore's count above 1
	 * (mirrors the original POSIX implementation's sem_getvalue() guard). */
	int expected = 0;
	if (atomic_compare_exchange_strong(&sem->available, &expected, 1)) {
		dispatch_semaphore_signal(sem->sem);
	}

	return CSP_SEMAPHORE_OK;
}

#else

#include <semaphore.h>
#include <time.h>

void csp_bin_sem_init(csp_bin_sem_t * sem) {

	sem_init((sem_t *) sem, 0, 1);
}

int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout) {

	int ret;

	if (timeout == CSP_MAX_TIMEOUT) {
		ret = sem_wait((sem_t *) sem);
	} else {
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts)) {
			return CSP_SEMAPHORE_ERROR;
		}

		uint32_t sec = timeout / 1000;
		uint32_t nsec = (timeout - 1000 * sec) * 1000000;

		ts.tv_sec += sec;

		if (ts.tv_nsec + nsec >= 1000000000) {
			ts.tv_sec++;
		}

		ts.tv_nsec = (ts.tv_nsec + nsec) % 1000000000;

		ret = sem_timedwait((sem_t *) sem, &ts);
	}

	if (ret != 0)
		return CSP_SEMAPHORE_ERROR;

	return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_post(csp_bin_sem_t * sem) {

	int value;
	sem_getvalue((sem_t *) sem, &value);
	if (value > 0) {
		return CSP_SEMAPHORE_OK;
	}

	if (sem_post((sem_t *) sem) == 0) {
		return CSP_SEMAPHORE_OK;
	}

	return CSP_SEMAPHORE_ERROR;
}

#endif
