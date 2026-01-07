#pragma once

#include <endian.h>
#include <stdint.h>

/**
 * Partially from Linux kernel source code (if_ether.h, ip.h, udp.h, tcp.h)
 */

#define ETH_ALEN 6

// ========== ETHERNET ==========

// proto value for IP
#define ETH_P_IP 0x800

typedef struct {
  uint8_t dest[ETH_ALEN];
  uint8_t src[ETH_ALEN];
  uint16_t proto;
} __attribute__((packed)) ethhdr_t;

_Static_assert(sizeof(ethhdr_t) == 14, "ethhdr_t size incorrect");

//
// ========== IP ==========
//

#define IP_P_TCP 6
#define IP_P_UDP 17

typedef uint32_t ipaddr_t;

typedef struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint8_t ihl : 4, version : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
  uint8_t version : 4, ihl : 4;
#endif
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t frag_off;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t chksum;
  ipaddr_t saddr;
  ipaddr_t daddr;
} __attribute__((packed)) iphdr_t;

_Static_assert(sizeof(iphdr_t) == 20, "iphdr_t size incorrect");

typedef uint16_t port_t;

//
// ========== UDP ==========
//

typedef struct {
  port_t source;
  port_t dest;
  uint16_t len;
  uint16_t chksum;
} __attribute__((packed)) udphdr_t;

//
// ========== TCP ==========
//

typedef struct {
  port_t source;
  port_t dest;
  uint32_t seq;
  uint32_t ack;
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint8_t reserved : 4;
  uint8_t doff : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
  uint8_t doff : 4;
  uint8_t reserved : 4;
#endif
  uint8_t flags;
  uint16_t window;
  uint16_t chksum;
  uint16_t urg_ptr;
} __attribute__((packed)) tcphdr_t;
