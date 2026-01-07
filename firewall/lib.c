#include <assert.h>
#include <endian.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

struct firewall {
  // Implement
};

firewall_t *firewall_create(void) {
  return NULL;
}
void firewall_destroy(firewall_t *firewall) {}

void firewall_add_mac_rule(firewall_t *firewall, uint8_t mac[],
                           action_t action) {}
void firewall_add_blacklist_rule(firewall_t *firewall, protocol_t proto,
                                 ipaddr_t srcip, ipaddr_t destip,
                                 port_t start_port, port_t end_port) {}
void firewall_add_content_rule(firewall_t *firewall, const char *pattern,
                               size_t pattern_len) {}
void firewall_configure_ratelimit(firewall_t *firewall, uint32_t rate_bps,
                                  uint64_t timeout_us) {}
action_t firewall_check(firewall_t *firewall, void *packet, size_t packet_len) {
  return ACTION_PASS;
}
