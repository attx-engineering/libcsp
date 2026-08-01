#pragma once

#include "csp/autoconfig.h"

#if (CSP_ZEPHYR)
#include <zephyr/kernel.h>
#else
#ifdef __APPLE__
/* Mach-O has no equivalent of the ELF .noinit section; it's an
 * embedded-target boot-time optimization with no benefit on macOS. */
#define __noinit
#else
#define __noinit __attribute__((section(".noinit")))
#endif
#define __packed __attribute__((__packed__))
#define __maybe_unused __attribute__((__unused__))
#define __unused __attribute__((__unused__))
#ifdef __CYGWIN__
#define __weak
#else
/* Clang (including AppleClang) predefines __weak for Objective-C ARC/GC;
 * redefine it to CSP's meaning without tripping -Werror=macro-redefined. */
#ifdef __weak
#undef __weak
#endif
#define __weak   __attribute__((__weak__))
#endif

#define CONTAINER_OF(ptr, type, member) \
	((type *)(void *)((char *)(ptr) - offsetof(type, member)))

#endif
