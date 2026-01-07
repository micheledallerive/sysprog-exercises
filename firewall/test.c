#include <arpa/inet.h>
#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../greatest.h"
#include "custom_tests.h"
#include "lib.h"
#include "net.h"

#define MAX_PACKET_SIZE 2048
#define RAW_BUFFER_SIZE (MAX_PACKET_SIZE + 2)

void parse_mac(const char *mac_str, uint8_t *mac_out) {
  sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac_out[0], &mac_out[1],
         &mac_out[2], &mac_out[3], &mac_out[4], &mac_out[5]);
}

void parse_ip(const char *ip_str, ipaddr_t *ip_out) {
  inet_pton(AF_INET, ip_str, ip_out);
  *ip_out = ntohl(*ip_out);
}

size_t build_packet(uint8_t *raw_buffer, uint8_t **packet_ptr,
                    const char *src_mac, const char *dest_mac,
                    const char *src_ip, const char *dest_ip, protocol_t proto,
                    uint16_t src_port, uint16_t dest_port,
                    const char *payload) {
  // Clear buffer
  memset(raw_buffer, 0, RAW_BUFFER_SIZE);

  // Offset by 2 bytes so that IP header (14 bytes in) lands on 16-byte boundary
  uint8_t *pkt = raw_buffer + 2;
  *packet_ptr = pkt;

  // 1. Ethernet Header
  ethhdr_t *eth = (ethhdr_t *)pkt;
  parse_mac(dest_mac, eth->dest);
  parse_mac(src_mac, eth->src);
  eth->proto = htons(ETH_P_IP);

  // 2. IP Header
  iphdr_t *ip = (iphdr_t *)(pkt + sizeof(ethhdr_t));
  size_t payload_len = payload ? strlen(payload) : 0;
  size_t l4_len = (proto == PROTOCOL_TCP) ? sizeof(tcphdr_t) : sizeof(udphdr_t);

  ip->ihl = 5;
  ip->version = 4;
  ip->tot_len = htons(sizeof(iphdr_t) + l4_len + payload_len);
  ip->ttl = 64;
  ip->protocol = (proto == PROTOCOL_TCP) ? IP_P_TCP : IP_P_UDP;
  inet_pton(AF_INET, src_ip, &ip->saddr);
  inet_pton(AF_INET, dest_ip, &ip->daddr);

  // 3. L4 Header (TCP/UDP)
  void *l4_ptr = (uint8_t *)ip + sizeof(iphdr_t);

  if (proto == PROTOCOL_TCP) {
    tcphdr_t *tcp = (tcphdr_t *)l4_ptr;
    tcp->source = htons(src_port);
    tcp->dest = htons(dest_port);
    tcp->doff = 5;  // Standard 20 byte header
  } else {
    udphdr_t *udp = (udphdr_t *)l4_ptr;
    udp->source = htons(src_port);
    udp->dest = htons(dest_port);
    udp->len = htons(sizeof(udphdr_t) + payload_len);
  }

  // 4. Payload
  if (payload) {
    memcpy((uint8_t *)l4_ptr + l4_len, payload, payload_len);
  }

  return sizeof(ethhdr_t) + sizeof(iphdr_t) + l4_len + payload_len;
}

// ==========================================
//          FEATURE 1: MAC RULES
// ==========================================

TEST test_mac_src_match() {
  firewall_t *fw = firewall_create();
  uint8_t bad_mac[6];
  parse_mac("aa:bb:cc:dd:ee:ff", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_dest_ignored() {
  firewall_t *fw = firewall_create();
  uint8_t bad_mac[6];
  parse_mac("aa:bb:cc:dd:ee:ff", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Bad MAC is in Destination, Source is safe -> PASS
  size_t len = build_packet(raw, &pkt, "11:22:33:44:55:66", "aa:bb:cc:dd:ee:ff",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_partial_match() {
  firewall_t *fw = firewall_create();
  uint8_t bad_mac[6];
  parse_mac("aa:bb:cc:dd:ee:ff", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Source differs by last byte (fe vs ff) -> PASS
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:fe", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_multiple_rules() {
  firewall_t *fw = firewall_create();
  uint8_t m1[6], m2[6];
  parse_mac("11:11:11:11:11:11", m1);
  parse_mac("22:22:22:22:22:22", m2);

  firewall_add_mac_rule(fw, m1, ACTION_DROP);
  firewall_add_mac_rule(fw, m2, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Matches second rule
  size_t len = build_packet(raw, &pkt, "22:22:22:22:22:22", "ff:ff:ff:ff:ff:ff",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_whitelist_pass() {
  firewall_t *fw = firewall_create();
  uint8_t good_mac[6];
  parse_mac("aa:bb:cc:dd:ee:ff", good_mac);
  // Explicit PASS rule (Whitelist scenario)
  firewall_add_mac_rule(fw, good_mac, ACTION_PASS);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_broadcast() {
  firewall_t *fw = firewall_create();
  uint8_t broadcast[6];
  parse_mac("ff:ff:ff:ff:ff:ff", broadcast);
  firewall_add_mac_rule(fw, broadcast, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Broadcast MAC should be blocked
  size_t len = build_packet(raw, &pkt, "ff:ff:ff:ff:ff:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_zeros() {
  firewall_t *fw = firewall_create();
  uint8_t zero_mac[6];
  parse_mac("00:00:00:00:00:00", zero_mac);
  firewall_add_mac_rule(fw, zero_mac, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Zero MAC should be blocked
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_no_rules_default() {
  firewall_t *fw = firewall_create();
  // No rules added
  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  // Default behavior should be pass
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_case_insensitivity_setup() {
  // Verifies that the parser/matcher handles hex values regardless of case
  // implied by underlying byte comparison
  firewall_t *fw = firewall_create();
  uint8_t mac[6];
  // Rule created with uppercase string
  parse_mac("AA:BB:CC:DD:EE:FF", mac);
  firewall_add_mac_rule(fw, mac, ACTION_DROP);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Packet created with lowercase string
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_mac_pass_before_drop() {
  firewall_t *fw = firewall_create();
  uint8_t mac[6];
  parse_mac("aa:bb:cc:dd:ee:ff", mac);

  // Rule order: First PASS, then DROP.
  // Since new rules are typically prepended (stack behavior) or appended,
  // this tests the list order logic.
  // Assuming `firewall_add` prepends (LIFO), we add DROP then PASS.
  firewall_add_mac_rule(fw, mac, ACTION_DROP);
  firewall_add_mac_rule(fw, mac, ACTION_PASS);  // This becomes head

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:bb:cc:dd:ee:ff", "11:22:33:44:55:66",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 80, 80, NULL);

  // If head is PASS, it should return PASS immediately
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

// ==========================================
//        FEATURE 2: BLACKLIST RULES
// ==========================================

TEST test_blacklist_exact_match() {
  firewall_t *fw = firewall_create();
  ipaddr_t src, dst;
  parse_ip("10.0.0.1", &src);
  parse_ip("10.0.0.2", &dst);

  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, src, dst, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "10.0.0.1", "10.0.0.2", PROTOCOL_TCP, 1234, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_port_range() {
  firewall_t *fw = firewall_create();
  ipaddr_t src, dst;
  parse_ip("10.0.0.1", &src);
  parse_ip("10.0.0.2", &dst);

  firewall_add_blacklist_rule(fw, PROTOCOL_UDP, src, dst, 1000, 2000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // Port 999 -> PASS
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "10.0.0.1", "10.0.0.2", PROTOCOL_UDP, 50, 999, NULL);
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Port 1500 -> DROP
  len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                     "10.0.0.1", "10.0.0.2", PROTOCOL_UDP, 50, 1500, NULL);
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_wildcards() {
  firewall_t *fw = firewall_create();
  // Rule: Drop TCP from ANY source (0) to ANY dest (0) on port 23
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 23, 23);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.2.3.4", "5.6.7.8", PROTOCOL_TCP, 6000, 23, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_wrong_proto() {
  firewall_t *fw = firewall_create();
  ipaddr_t src, dst;
  parse_ip("10.0.0.1", &src);
  parse_ip("10.0.0.2", &dst);

  // Rule is for TCP
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, src, dst, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Packet is UDP -> PASS
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "10.0.0.1", "10.0.0.2", PROTOCOL_UDP, 1234, 80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_similar_ip() {
  firewall_t *fw = firewall_create();
  ipaddr_t src, dst;
  parse_ip("192.168.1.10", &src);
  parse_ip("192.168.1.20", &dst);

  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, src, dst, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Source is 192.168.1.11 (not .10) -> PASS
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "192.168.1.11", "192.168.1.20", PROTOCOL_TCP, 1234,
                            80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_src_wildcard_only() {
  firewall_t *fw = firewall_create();
  ipaddr_t dst;
  parse_ip("192.168.1.5", &dst);
  // Source = 0 (Any), Dest = Specific
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, dst, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "192.168.1.5", PROTOCOL_TCP, 1234, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_dest_wildcard_only() {
  firewall_t *fw = firewall_create();
  ipaddr_t src;
  parse_ip("192.168.1.5", &src);
  // Source = Specific, Dest = 0 (Any)
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, src, 0, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "192.168.1.5", "8.8.8.8", PROTOCOL_TCP, 1234, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_boundary_ports() {
  firewall_t *fw = firewall_create();
  // Range [100, 200]
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 100, 200);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // Test Exact Start (100) -> DROP
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "1.1.1.1", PROTOCOL_TCP, 999, 100, NULL);
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  // Test Exact End (200) -> DROP
  len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                     "1.1.1.1", "1.1.1.1", PROTOCOL_TCP, 999, 200, NULL);
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_inverted_range() {
  firewall_t *fw = firewall_create();
  // Start > End (Invalid range logic, but safe code should just not match it)
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 200, 100);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Port 150 is between 100 and 200, but logic usually requires start <= port
  // <= end If start=200, end=100. 200 <= 150 AND 150 <= 100 -> False. Should
  // PASS.
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "1.1.1.1", PROTOCOL_TCP, 999, 150, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_blacklist_max_port() {
  firewall_t *fw = firewall_create();
  // Test boundary at 65535
  firewall_add_blacklist_rule(fw, PROTOCOL_UDP, 0, 0, 65535, 65535);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "1.1.1.1", PROTOCOL_UDP, 999, 65535, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

// ==========================================
//        FEATURE 3: CONTENT RULES
// ==========================================

TEST test_content_exact_match() {
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "virus", 5);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "virus");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_partial_match_should_pass() {
  // Spec: "matches 'pattern' _exactly_" implies the WHOLE payload must equal
  // pattern.
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "virus", 5);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload contains "virus" but is not exactly "virus" -> PASS
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "virus_v2");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_empty_payload() {
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "bad", 3);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Empty payload should not match
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, NULL);

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_udp_support() {
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "secret", 6);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Should work for UDP as well
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_UDP, 80, 80, "secret");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_header_offset() {
  // Ensures we are skipping IP/TCP headers correctly
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "ABC", 3);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // If we scanned headers, we might find bytes matching "ABC".
  // Payload "XYZ" should pass.
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "XYZ");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_length_mismatch() {
  firewall_t *fw = firewall_create();
  // Pattern "hell", Payload "hello".
  // Should NOT match because exact match requires lengths to be equal
  firewall_add_content_rule(fw, "hell", 4);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "hello");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_case_sensitivity() {
  firewall_t *fw = firewall_create();
  // Rules are usually exact byte matches. "Virus" != "virus".
  firewall_add_content_rule(fw, "Virus", 5);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "virus");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_binary() {
  firewall_t *fw = firewall_create();
  // Pattern contains null byte: "A\0B"
  char pattern[3] = {'A', 0, 'B'};
  firewall_add_content_rule(fw, pattern, 3);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // build_packet uses strlen, so we construct payload manually for this test
  char payload[3] = {'A', 0, 'B'};

  // Create dummy packet with 3 bytes
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "XXX");

  // Overwrite the payload area with binary data
  // eth(14) + ip(20) + tcp(20) = 54 bytes offset
  memcpy(pkt + 54, payload, 3);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_content_multi_rule() {
  firewall_t *fw = firewall_create();
  firewall_add_content_rule(fw, "AAAA", 4);
  firewall_add_content_rule(fw, "BBBB", 4);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // Match second rule
  size_t len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "BBBB");
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  // Match first rule
  len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                     "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, "AAAA");
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_content_large_payload_exact() {
  firewall_t *fw = firewall_create();
  // 1000 bytes pattern
  char pattern[1000];
  memset(pattern, 'X', 1000);
  firewall_add_content_rule(fw, pattern, 1000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Create packet with 1001 bytes payload (add null terminator for string
  // safety in helper)
  char payload[1001];
  memset(payload, 'X', 1000);
  payload[1000] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // Exact match of 1000 bytes
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

// ==========================================
//        FEATURE 4: RATE LIMIT RULES
// ==========================================

TEST test_ratelimit_drop_capacity() {
  firewall_t *fw = firewall_create();
  // Rate 1000 B/s.
  firewall_configure_ratelimit(fw, 1000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[1001];
  memset(payload, 'A', 1000);
  payload[1000] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // 1. Packet 1: 1000 bytes. Bucket = 0. 0 < 1000 -> PASS. New Bucket = 1000.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 2. Packet 2: 1000 bytes. Bucket = 1000. 1000 >= 1000 -> DROP.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_accumulation() {
  firewall_t *fw = firewall_create();
  // Rate 2000 B/s.
  firewall_configure_ratelimit(fw, 2000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[501];
  memset(payload, 'A', 500);
  payload[500] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // 0 -> 500
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // 500 -> 1000
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // 1000 -> 1500
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // 1500 -> 2000
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // 2000 -> Drop (since B_F >= R)
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_drain() {
  firewall_t *fw = firewall_create();
  // Rate 1000 B/s.
  firewall_configure_ratelimit(fw, 1000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[801];
  memset(payload, 'A', 800);
  payload[800] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // Fill 800
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Wait 1.1s. Drain should clear the bucket (1100 bytes drained).
  usleep(1100000);

  // Send another 800. Should PASS (Bucket was 0).
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_flow_isolation() {
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  char payload[801];
  memset(payload, 'A', 800);
  payload[800] = '\0';
  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // Flow 1
  size_t len1 =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len1));  // F1: 800

  // Flow 2 (Different Src IP)
  uint8_t raw2[RAW_BUFFER_SIZE];
  uint8_t *pkt2;
  size_t len2 =
      build_packet(raw2, &pkt2, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "9.9.9.9", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // Should PASS (Flow 2 bucket is 0)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt2, len2));  // F2: 800

  // Overfill Flow 1 -> DROP
  ASSERT_EQ(ACTION_DROP,
            firewall_check(fw, pkt, len1));  // F1: 1600 > 1000 -> Drop

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_cleanup_timeout() {
  firewall_t *fw = firewall_create();
  // Rate 1000, Timeout 0.5s
  firewall_configure_ratelimit(fw, 1000, 500000);

  char payload[801];
  memset(payload, 'A', 800);
  payload[800] = '\0';
  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // 1. Fill bucket to 800
  firewall_check(fw, pkt, len);

  // 2. Wait 0.6s. State should expire/be removed.
  usleep(600000);

  // 3. Send again. If state was removed, new state starts at 0 -> 800 (PASS).
  // If state persisted but drain calc was wrong, it might fail.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_single_large_packet() {
  firewall_t *fw = firewall_create();
  // Rate 500 bytes/sec
  firewall_configure_ratelimit(fw, 500, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload 600 bytes
  char payload[601];
  memset(payload, 'A', 600);
  payload[600] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // Packet size (600) > Rate (500). Should drop immediately.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_rapid_small_packets() {
  firewall_t *fw = firewall_create();
  // Rate 100 bytes/sec
  firewall_configure_ratelimit(fw, 100, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload 10 bytes
  char payload[11];
  memset(payload, 'A', 10);
  payload[10] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // Send 10 packets of 10 bytes = 100 bytes (PASS)
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  }

  // 11th packet = 110 bytes total (DROP)
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_zero_rate() {
  firewall_t *fw = firewall_create();
  // Rate 0 bytes/sec -> Block everything with payload
  firewall_configure_ratelimit(fw, 0, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[10];
  memset(payload, 'A', 9);
  payload[9] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_exact_capacity_edge() {
  firewall_t *fw = firewall_create();
  // Rate 100
  firewall_configure_ratelimit(fw, 100, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload 50
  char payload[51];
  memset(payload, 'A', 50);
  payload[50] = '\0';
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 10, 20, payload);

  // 50 < 100 -> PASS. Bucket = 50.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 50 + 50 = 100. 100 <= 100 ?
  // Code says: if (bucket + payload > rate) DROP.
  // 50 + 50 = 100. 100 is NOT > 100. So PASS.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Next packet. Bucket = 100. 100 + 50 = 150 > 100. DROP.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_udp_flow() {
  // Verify UDP packets are tracked in separate flows if everything else
  // matches, or same flow? Key is 4-tuple (srcIP, srcPort, dstIP, dstPort).
  // Protocol is NOT in the tuple description in requirements. However, TCP and
  // UDP header parsing differs. If srcIP, srcPort, dstIP, dstPort are
  // identical, they collide.

  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  char payload[600];
  memset(payload, 'A', 599);
  payload[599] = '\0';
  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // TCP Flow
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));  // bucket = 600

  // UDP Packet with SAME IP/Port tuple
  // Since requirements say flow is 4-tuple, this shares the bucket.
  len = build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                     "1.1.1.1", "2.2.2.2", PROTOCOL_UDP, 80, 80, payload);

  // 600 + 600 = 1200 > 1000 -> DROP
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

// ==========================================
//      ADDITIONAL RATE LIMIT TESTS
// ==========================================

TEST test_ratelimit_header_overhead_exclusion() {
  // Requirement: "B_F is increased by the bytes in the IP payload (i.e. don't
  // count the IP header!)" We should also ensure TCP/UDP headers are not
  // counted if "IP payload" implies L4 data, but usually "IP Payload" means
  // everything after the IP header (L4 Header + Data). However, standard leaky
  // buckets for apps often count L7 data. Let's assume the implementation
  // subtracts L4 headers based on the code provided previously (which
  // calculates payload_len = total - eth - ip - l4).

  firewall_t *fw = firewall_create();
  // Rate 100 bytes/sec
  firewall_configure_ratelimit(fw, 100, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload 80 bytes.
  // Headers: Eth(14) + IP(20) + TCP(20) = 54 bytes.
  // Total packet size = 134 bytes.
  char payload[81];
  memset(payload, 'A', 80);
  payload[80] = '\0';

  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // If headers were counted: 134 > 100 -> DROP
  // If headers excluded: 80 <= 100 -> PASS
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Send another 80 bytes.
  // Bucket = 80. 80 + 80 = 160 > 100 -> DROP.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_directionality() {
  // Ensure A->B and B->A are treated as separate flows
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[800];
  memset(payload, 'A', 800);
  // No null terminator needed for memsets if not passed as string,
  // but build_packet expects string.
  payload[799] = '\0';

  // Flow 1: 1.1.1.1 -> 2.2.2.2
  size_t len1 =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len1));  // F1 Bucket: 800

  // Flow 2: 2.2.2.2 -> 1.1.1.1 (Reverse)
  uint8_t raw2[RAW_BUFFER_SIZE];
  uint8_t *pkt2;
  size_t len2 =
      build_packet(raw2, &pkt2, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "2.2.2.2", "1.1.1.1", PROTOCOL_TCP, 80, 80, payload);

  // Should PASS (New bucket). If directionality was ignored, 800+800 > 1000 ->
  // Drop
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt2, len2));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_port_swapped_isolation() {
  // Ensure 1.1.1.1:80 -> 2.2.2.2:90 is different from 1.1.1.1:90 -> 2.2.2.2:80
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  char payload[800];
  memset(payload, 'A', 799);
  payload[799] = '\0';

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;

  // Flow A: SrcPort 100, DstPort 200
  size_t lenA =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 200, payload);
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, lenA));

  // Flow B: SrcPort 200, DstPort 100 (Swapped ports, same IPs)
  uint8_t rawB[RAW_BUFFER_SIZE];
  uint8_t *pktB;
  size_t lenB =
      build_packet(rawB, &pktB, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 200, 100, payload);

  // Should PASS (New bucket)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pktB, lenB));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_interleaved_flows() {
  // Verify that multiple flows active simultaneously don't corrupt each other
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  char payload[400];
  memset(payload, 'A', 399);
  payload[399] = '\0';

  uint8_t rawA[RAW_BUFFER_SIZE], rawB[RAW_BUFFER_SIZE];
  uint8_t *pktA, *pktB;

  // Flow A: 1.1.1.1
  size_t lenA =
      build_packet(rawA, &pktA, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);
  // Flow B: 3.3.3.3
  size_t lenB =
      build_packet(rawB, &pktB, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "3.3.3.3", "4.4.4.4", PROTOCOL_TCP, 80, 80, payload);

  // A: 400 (Bucket A=400)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pktA, lenA));
  // B: 400 (Bucket B=400)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pktB, lenB));
  // A: 400 (Bucket A=800)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pktA, lenA));
  // B: 400 (Bucket B=800)
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pktB, lenB));

  // A: 400 (Bucket A=1200 > 1000) -> DROP
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pktA, lenA));

  // B: 400 (Bucket B=1200 > 1000) -> DROP
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pktB, lenB));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_ip_options_handling() {
  // Edge Case: IP Header > 20 bytes (IHL > 5).
  // If the implementation assumes IHL=5 (20 bytes), it will read the wrong
  // offset for ports and calculate the payload size incorrectly.

  firewall_t *fw = firewall_create();
  // Rate 100 bytes/sec
  firewall_configure_ratelimit(fw, 100, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  char payload[101];  // 100 bytes 'A'
  memset(payload, 'A', 100);
  payload[100] = '\0';

  // 1. Build standard packet
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // 2. MANUALLY INJECT IP OPTIONS
  // Increase IHL from 5 to 6 (adds 4 bytes to header)
  iphdr_t *ip = (iphdr_t *)(pkt + sizeof(ethhdr_t));
  ip->ihl = 6;

  // Shift everything after IP header (L4 + payload) by 4 bytes to make room
  // Original IP end: 14 + 20 = 34.
  // Move from 34 onwards to 38.
  size_t bytes_to_move = len - (sizeof(ethhdr_t) + 20);  // Total - 34
  memmove(pkt + sizeof(ethhdr_t) + 24, pkt + sizeof(ethhdr_t) + 20,
          bytes_to_move);

  // Update offsets
  len += 4;  // Total packet is 4 bytes larger

  // Total Payload Size (Bytes tracked) should still be 100.
  // Calculation: Total(138) - Eth(14) - IP(24) - TCP(20) = 80 ?
  // Wait, build_packet makes TCP header.
  // Payload len = 100.
  // 100 <= 100 -> PASS.

  // If the code blindly uses sizeof(iphdr_t)=20:
  // It thinks L4 starts at 34. But L4 starts at 38.
  // It reads garbage for ports (Flow ID corruption).
  // It calculates payload_len = Total(138) - 14 - 20 - 20 = 84 bytes?
  // Or it thinks the 4 bytes of options are part of L4/Payload.

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Send identical second packet. Should DROP (100+100 > 100).
  // If Flow ID corrupted, it might PASS (different garbage ports).
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_partial_drain() {
  // Fill bucket, wait 50% of time, fill rest.
  firewall_t *fw = firewall_create();
  // Rate 1000 B/s.
  firewall_configure_ratelimit(fw, 1000, 2000000);  // 2s timeout

  char payload[501];
  memset(payload, 'A', 500);
  payload[500] = '\0';

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // 1. Send 500. Bucket = 500.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 2. Send 500. Bucket = 1000.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 3. Bucket Full (1000). Next packet drops.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  // 4. Sleep 0.5 seconds.
  // Drain = Rate * Time = 1000 * 0.5 = 500 bytes.
  // Bucket should go 1000 -> 500.
  usleep(500000);

  // 5. Send 500. Bucket 500 + 500 = 1000. Should PASS.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 6. Bucket Full again.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_clamped_drain() {
  // Ensure bucket doesn't go negative.
  firewall_t *fw = firewall_create();
  // Rate 1000 B/s
  firewall_configure_ratelimit(fw, 1000, 2000000);

  char payload[501];
  memset(payload, 'A', 500);
  payload[500] = '\0';

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // 1. Send 500. Bucket = 500.
  firewall_check(fw, pkt, len);

  // 2. Sleep 2.0 seconds. Drain 2000 bytes.
  // Bucket 500 - 2000 = -1500? NO, must clamp to 0.
  usleep(2000000);

  // 3. Send 1000 bytes (Two 500 packets).
  // If bucket was -1500, we could send 2500 bytes.
  // If bucket is 0, we can send exactly 1000.

  // Packet 1 (500) -> PASS. Bucket 500.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // Packet 2 (500) -> PASS. Bucket 1000.
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  // Packet 3 (500) -> DROP.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_burst_strictly_n() {
  // Verify exact number of packets pass in a burst
  firewall_t *fw = firewall_create();
  // Rate 500. Packet payload 100. Should allow exactly 5 packets.
  firewall_configure_ratelimit(fw, 500, 1000000);

  char payload[101];
  memset(payload, 'A', 100);
  payload[100] = '\0';

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  }
  // 6th packet -> 600 > 500 -> Drop
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_tiny_limit() {
  // Boundary condition: Rate 1 byte/sec.
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload 1 byte
  char payload[2] = {'A', '\0'};
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 80, 80, payload);

  // 1 <= 1 -> PASS
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // 1 + 1 = 2 > 1 -> DROP
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_ratelimit_self_loop() {
  // Source IP == Dest IP. Source Port == Dest Port.
  // Valid flow, ensure no logic error in hashing/matching equality.
  firewall_t *fw = firewall_create();
  firewall_configure_ratelimit(fw, 1000, 1000000);

  char payload[600];
  memset(payload, 'A', 599);
  payload[599] = '\0';

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "127.0.0.1", "127.0.0.1", PROTOCOL_TCP, 5000, 5000, payload);

  // First packet (600) -> PASS
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));

  // Second packet (1200 > 1000) -> DROP
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_combined_mac_drop_blacklist_pass() {
  // Scenario: MAC rule says DROP. No Blacklist rule matches (default PASS).
  // Result: DROP.
  firewall_t *fw = firewall_create();

  uint8_t bad_mac[6];
  parse_mac("aa:aa:aa:aa:aa:aa", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  // Add an unrelated blacklist rule just to populate the list
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 9000, 9000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Matches MAC rule. Port 80 does NOT match blacklist rule.
  size_t len = build_packet(raw, &pkt, "aa:aa:aa:aa:aa:aa", "11:22:33:44:55:66",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_mac_pass_blacklist_drop() {
  // Scenario: MAC rule says PASS (whitelist). Blacklist rule says DROP.
  // Result: DROP (Pass should not override Drop).
  firewall_t *fw = firewall_create();

  uint8_t good_mac[6];
  parse_mac("aa:aa:aa:aa:aa:aa", good_mac);
  firewall_add_mac_rule(fw, good_mac, ACTION_PASS);

  // Blacklist drop on port 80
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 80, 80);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:aa:aa:aa:aa:aa", "11:22:33:44:55:66",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, NULL);

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_blacklist_drop_content_pass() {
  // Scenario: Blacklist matches (DROP). Content does not match (PASS).
  // Result: DROP.
  firewall_t *fw = firewall_create();

  // Drop port 80
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 80, 80);

  // Content rule exists for "virus", but packet has "clean"
  firewall_add_content_rule(fw, "virus", 5);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "clean");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_blacklist_pass_content_drop() {
  // Scenario: Blacklist allows (Port 8080). Content matches "virus" (DROP).
  // Result: DROP.
  firewall_t *fw = firewall_create();

  // Drop port 80 (Packet is 8080, so Blacklist passes)
  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 80, 80);

  firewall_add_content_rule(fw, "virus", 5);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 8080, "virus");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_content_pass_ratelimit_drop() {
  // Scenario: Content is safe. Rate limit exceeded.
  // Result: DROP.
  firewall_t *fw = firewall_create();

  firewall_add_content_rule(fw, "virus", 5);
  // Low rate limit
  firewall_configure_ratelimit(fw, 10, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload "safe" (4 bytes).
  // Packet 1: 4 bytes (Pass). Packet 2: 8 bytes (Pass). Packet 3: 12 bytes > 10
  // (Drop).
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "safe");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_combined_content_drop_ratelimit_pass() {
  // Scenario: Content matches "virus" (DROP). Rate limit is fine.
  // Result: DROP.
  firewall_t *fw = firewall_create();

  firewall_add_content_rule(fw, "virus", 5);
  // High rate limit
  firewall_configure_ratelimit(fw, 1000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "virus");

  // Even though bucket is empty, content triggers drop.
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_combined_ratelimit_drop_mac_pass() {
  // Scenario: Rate limit exceeded (DROP). MAC explicitly allowed (PASS).
  // Result: DROP.
  firewall_t *fw = firewall_create();

  uint8_t good_mac[6];
  parse_mac("aa:aa:aa:aa:aa:aa", good_mac);
  firewall_add_mac_rule(fw, good_mac, ACTION_PASS);

  firewall_configure_ratelimit(fw, 10, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Payload "load" (4 bytes).
  // Packet 3 will exceed rate 10.
  size_t len =
      build_packet(raw, &pkt, "aa:aa:aa:aa:aa:aa", "11:22:33:44:55:66",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "load");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));

  firewall_destroy(fw);
  PASS();
}

TEST test_combined_all_drop() {
  // Scenario: MAC, Blacklist, and Content ALL match and say DROP.
  // Result: DROP.
  firewall_t *fw = firewall_create();

  uint8_t bad_mac[6];
  parse_mac("aa:aa:aa:aa:aa:aa", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 80, 80);

  firewall_add_content_rule(fw, "bad", 3);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len = build_packet(raw, &pkt, "aa:aa:aa:aa:aa:aa", "11:22:33:44:55:66",
                            "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "bad");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_all_pass() {
  // Scenario: Rules exist for MAC, Blacklist, Content, Rate. Packet evades all
  // of them. Result: PASS.
  firewall_t *fw = firewall_create();

  uint8_t bad_mac[6];
  parse_mac("ff:ff:ff:ff:ff:ff", bad_mac);
  firewall_add_mac_rule(fw, bad_mac, ACTION_DROP);

  firewall_add_blacklist_rule(fw, PROTOCOL_TCP, 0, 0, 9000, 9000);
  firewall_add_content_rule(fw, "virus", 5);
  firewall_configure_ratelimit(fw, 1000, 1000000);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  // Clean MAC, Port 80, Content "clean", Size 5 bytes.
  size_t len =
      build_packet(raw, &pkt, "aa:aa:aa:aa:aa:aa", "11:22:33:44:55:66",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "clean");

  ASSERT_EQ(ACTION_PASS, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

TEST test_combined_protocol_mismatch_content_drop() {
  // Scenario: Blacklist rule is for UDP (so TCP passes it).
  // But content rule matches (Applies to both TCP/UDP).
  // Result: DROP.
  firewall_t *fw = firewall_create();

  // Blacklist UDP port 80. Packet is TCP port 80.
  firewall_add_blacklist_rule(fw, PROTOCOL_UDP, 0, 0, 80, 80);

  firewall_add_content_rule(fw, "fail", 4);

  uint8_t raw[RAW_BUFFER_SIZE];
  uint8_t *pkt;
  size_t len =
      build_packet(raw, &pkt, "00:00:00:00:00:00", "00:00:00:00:00:00",
                   "1.1.1.1", "2.2.2.2", PROTOCOL_TCP, 100, 80, "fail");

  ASSERT_EQ(ACTION_DROP, firewall_check(fw, pkt, len));
  firewall_destroy(fw);
  PASS();
}

// ==========================================
//                TEST RUNNER
// ==========================================

SUITE(suite_mac) {
  RUN_TEST(test_mac_src_match);
  RUN_TEST(test_mac_dest_ignored);
  RUN_TEST(test_mac_partial_match);
  RUN_TEST(test_mac_multiple_rules);
  RUN_TEST(test_mac_whitelist_pass);
  // New tests
  RUN_TEST(test_mac_broadcast);
  RUN_TEST(test_mac_zeros);
  RUN_TEST(test_mac_no_rules_default);
  RUN_TEST(test_mac_case_insensitivity_setup);
  RUN_TEST(test_mac_pass_before_drop);
}

SUITE(suite_blacklist) {
  RUN_TEST(test_blacklist_exact_match);
  RUN_TEST(test_blacklist_port_range);
  RUN_TEST(test_blacklist_wildcards);
  RUN_TEST(test_blacklist_wrong_proto);
  RUN_TEST(test_blacklist_similar_ip);
  // New tests
  RUN_TEST(test_blacklist_src_wildcard_only);
  RUN_TEST(test_blacklist_dest_wildcard_only);
  RUN_TEST(test_blacklist_boundary_ports);
  RUN_TEST(test_blacklist_inverted_range);
  RUN_TEST(test_blacklist_max_port);
}

SUITE(suite_content) {
  RUN_TEST(test_content_exact_match);
  RUN_TEST(test_content_partial_match_should_pass);
  RUN_TEST(test_content_empty_payload);
  RUN_TEST(test_content_udp_support);
  RUN_TEST(test_content_header_offset);
  // New tests
  RUN_TEST(test_content_length_mismatch);
  RUN_TEST(test_content_case_sensitivity);
  RUN_TEST(test_content_binary);
  RUN_TEST(test_content_multi_rule);
  RUN_TEST(test_content_large_payload_exact);
}

SUITE(suite_ratelimit) {
  RUN_TEST(test_ratelimit_drop_capacity);
  RUN_TEST(test_ratelimit_accumulation);
  RUN_TEST(test_ratelimit_drain);
  RUN_TEST(test_ratelimit_flow_isolation);
  RUN_TEST(test_ratelimit_cleanup_timeout);
  // New tests
  RUN_TEST(test_ratelimit_single_large_packet);
  RUN_TEST(test_ratelimit_rapid_small_packets);
  RUN_TEST(test_ratelimit_zero_rate);
  RUN_TEST(test_ratelimit_exact_capacity_edge);
  RUN_TEST(test_ratelimit_udp_flow);
  // Additional tests
  RUN_TEST(test_ratelimit_header_overhead_exclusion);
  RUN_TEST(test_ratelimit_directionality);
  RUN_TEST(test_ratelimit_port_swapped_isolation);
  RUN_TEST(test_ratelimit_interleaved_flows);
  RUN_TEST(test_ratelimit_ip_options_handling);
  RUN_TEST(test_ratelimit_partial_drain);
  RUN_TEST(test_ratelimit_clamped_drain);
  RUN_TEST(test_ratelimit_burst_strictly_n);
  RUN_TEST(test_ratelimit_tiny_limit);
  RUN_TEST(test_ratelimit_self_loop);
}

SUITE(suite_combined) {
  RUN_TEST(test_combined_mac_drop_blacklist_pass);
  RUN_TEST(test_combined_mac_pass_blacklist_drop);
  RUN_TEST(test_combined_blacklist_drop_content_pass);
  RUN_TEST(test_combined_blacklist_pass_content_drop);
  RUN_TEST(test_combined_content_pass_ratelimit_drop);
  RUN_TEST(test_combined_content_drop_ratelimit_pass);
  RUN_TEST(test_combined_ratelimit_drop_mac_pass);
  RUN_TEST(test_combined_all_drop);
  RUN_TEST(test_combined_all_pass);
  RUN_TEST(test_combined_protocol_mismatch_content_drop);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_mac);
  RUN_SUITE(suite_blacklist);
  RUN_SUITE(suite_content);
  RUN_SUITE(suite_ratelimit);
  RUN_SUITE(suite_combined);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
