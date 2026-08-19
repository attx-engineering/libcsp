#pragma once

#include <csp/csp.h>
#include <csp/csp_interface.h>

#if (CSP_USE_RDP)
#define FIFO_TIMEOUT 100  //! If RDP is enabled, the router needs to awake some times to check timeouts
#else
#define FIFO_TIMEOUT CSP_MAX_TIMEOUT  //! If no RDP, the router can sleep until data arrives
#endif

/**
 * Init FIFO/QOS queues
 * @return CSP_ERR type
 */
void csp_qfifo_init(void);

typedef struct {
	csp_iface_t * iface;
	csp_packet_t * packet;
} csp_qfifo_t;

/**
 * Read next packet from router input queue
 * @param input pointer to router queue item element
 * @return CSP_ERR type
 */
int csp_qfifo_read(csp_qfifo_t * input);

/**
 * Read next packet from router input queue, without blocking.
 * Identical to csp_qfifo_read() except that an empty queue returns
 * #CSP_ERR_TIMEDOUT immediately rather than waiting FIFO_TIMEOUT for a packet
 * to arrive. For callers that poll the router from an existing task instead of
 * running it as a dedicated blocking router task.
 * @param input pointer to router queue item element
 * @return CSP_ERR type
 */
int csp_qfifo_read_noblock(csp_qfifo_t * input);

/**
 * Wake up any task (e.g. router) waiting on messages.
 * For testing.
 */
void csp_qfifo_wake_up(void);
