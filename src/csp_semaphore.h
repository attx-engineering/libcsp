

#pragma once

#include "csp/autoconfig.h"

#define CSP_SEMAPHORE_OK 	0
#define CSP_SEMAPHORE_ERROR	-1

#if (CSP_POSIX || __DOXYGEN__)
    #include <semaphore.h>
    #ifdef __APPLE__
        /* macOS lacks unnamed POSIX semaphores (sem_init() -> ENOSYS),
         * sem_timedwait() (not implemented at all), and deprecates
         * sem_getvalue(). Use Grand Central Dispatch instead -- see
         * arch/posix/csp_semaphore.c. The atomic mirrors the semaphore's
         * logical value (0 or 1) so post() can replicate the original
         * binary-cap behaviour (skip posting if already signaled). */
        #include <dispatch/dispatch.h>
        #ifdef __cplusplus
        /* Raw C11 <stdatomic.h> conflicts with libc++'s atomic_flag when
         * pulled into a C++ translation unit under Apple Clang. Use
         * std::atomic<int> directly rather than a `using std::atomic_int;`
         * alias -- some existing flight code already typedefs its own
         * (non-atomic) atomic_int, which a `using` here would collide with. */
        #include <atomic>
        #else
        #include <stdatomic.h>
        #endif
        typedef struct {
            dispatch_semaphore_t sem;
        #ifdef __cplusplus
            std::atomic<int> available;
        #else
            atomic_int available;
        #endif
        } csp_bin_sem_t;
    #else
        typedef sem_t csp_bin_sem_t;
    #endif
#elif (CSP_FREERTOS)
    #include <FreeRTOS.h>
    #include <task.h>
    typedef TaskHandle_t csp_bin_sem_t;
#elif (CSP_ZEPHYR)
    #include <zephyr/kernel.h>
    typedef struct k_sem csp_bin_sem_t;
#endif

/**
 * initialize a binary semaphore with static storage
 * The semaphore is created in state \a unlocked (value 1).
 * On platforms supporting max values, the semaphore is created with a max value of 1, hence the naming \a binary.
 */
void csp_bin_sem_init(csp_bin_sem_t * sem);

/**
 * Wait/lock semaphore
 * @param[in] timeout timeout in mS. Use #CSP_MAX_TIMEOUT for no timeout, e.g. wait forever until locked.
 * @return #CSP_SEMAPHORE_OK on success, otherwise #CSP_SEMAPHORE_ERROR
 */
int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout);

/**
 * Signal/unlock semaphore
 * @return #CSP_SEMAPHORE_OK on success, otherwise #CSP_SEMAPHORE_ERROR
 */
int csp_bin_sem_post(csp_bin_sem_t * sem);
