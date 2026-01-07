#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "net.h"

typedef enum { ACTION_PASS = 0, ACTION_DROP } action_t;
typedef enum { PROTOCOL_TCP = 0, PROTOCOL_UDP, PROTOCOL_OTHER } protocol_t;

typedef struct firewall firewall_t;

static inline uint64_t timestamp_us(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

firewall_t *firewall_create(void);
void firewall_destroy(firewall_t *firewall);

/**
 * Whenever the source mac address of a packet matches the given mac, apply
 * the given action
 */
void firewall_add_mac_rule(firewall_t *firewall, uint8_t mac[],
                           action_t action);

/**
 * The firewall should drop all packets from srcip to destip with destination
 * port in the range [start_port, end_port]. If srcip == 0, then the rule
 * applies to all sources. If destip == 0, then the rule applies to all
 * destinations. [start_port, end_port] is always a valid range.
 */
void firewall_add_blacklist_rule(firewall_t *firewall, protocol_t proto,
                                 ipaddr_t srcip, ipaddr_t destip,
                                 port_t start_port, port_t end_port);

/**
 * Drop packets that contain a payload that matches "pattern" _exactly_.
 * This rule applies to both UDP and TCP.
 * The "pattern" butter is owned by the caller, so the content should be copied
 * into a buffer owned by the firewall.
 */
void firewall_add_content_rule(firewall_t *firewall, const char *pattern,
                               size_t pattern_len);

/**
 * Configures the firewall to apply a Leaky Bucket rate limit on individual
 * flows.
 *
 * A "flow" is identified by the unique 4-tuple:
 * (Source IP, Source Port, Destination IP, Destination Port).
 *
 * Leaky Bucket (LB) Semantics:
 * 1. Each flow F has its own bucket B_F, tracking IP payload bytes.
 * 2. B_F starts at 0.
 * 3. If a packet is received and B_F >= R, the packet should be dropped.
 * 4. For every packet that is not dropped, B_F is increased by the bytes in the
 * IP payload (i.e. don't count the IP header!).
 * 5. At the same time, B_F is reduced at a rate of rate_bps bytes per second.
 *
 * A flow is considered terminated if no packets are received for 'timeout_us'
 * microseconds.
 *
 * You can get the current timestamp with the provided timestamp_us()
 * method.
 */
void firewall_configure_ratelimit(firewall_t *firewall, uint32_t rate_bps,
                                  uint64_t timeout_us);

action_t firewall_check(firewall_t *firewall, void *packet, size_t packet_len);

#endif  // LIB_H
