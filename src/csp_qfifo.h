#pragma once

#include <csp/csp.h>
#include <csp/csp_interface.h>

#if (CSP_USE_RDP)
// VENDORED-CODE MODIFICATION (not upstream libcsp): reduced from 100ms. csp_route_work() is
// called synchronously, per RX poll, from the single cooperative task running this project's
// entire flight schedule (see LibCspBackendCan::_routeAndDispatch()) -- not from libcsp's
// usual dedicated router task, where a 100ms idle sleep is harmless. Any csp_route_work()
// call that finds the qfifo empty blocks this FIFO_TIMEOUT, stalling the whole FSW schedule,
// not just CAN RX. 1ms caps that stall at this project's own worst-case tolerance. Safe to
// shrink: this deployment never uses RDP (no csp_connect()/RDP socket options anywhere in
// src/ or package/), so the only cost is checking connection timeouts 1000x/sec instead of
// 10x/sec against what is normally zero active RDP connections. Must be reapplied if libcsp
// is ever re-vendored from upstream.
#define FIFO_TIMEOUT 1
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
 * Wake up any task (e.g. router) waiting on messages.
 * For testing.
 */
void csp_qfifo_wake_up(void);
