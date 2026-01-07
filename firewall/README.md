# Software Firewall

## Goal

Implement a **Software Firewall** library capable of inspecting network packets and enforcing security rules.

You must implement the library defined in `lib.h`. Your firewall will process raw byte buffers representing Ethernet frames, parse the headers (Ethernet, IP, TCP/UDP), and make decisions (`ACTION_PASS` or `ACTION_DROP`) based on a set of configurable rules.

---

## Constraints and Requirements

* **Network Protocols**: You will work with raw packet data. You must correctly parse `ethhdr_t`, `iphdr_t`, `tcphdr_t`, and `udphdr_t` structures defined in `net.h`.
* **Byte Ordering**: Network protocols use **Big Endian** (Network Byte Order). Your system likely uses **Little Endian** (Host Byte Order). You must handle conversions using `ntohs`, `ntohl`, `be16toh`, etc., where appropriate.
* **Performance**: Your firewall does NOT need to be optimized for performance, but it should handle typical packet inspection tasks efficiently and, most importantly, correctly.

---

## Core API

### Lifecycle
* **`firewall_create`**: Allocates and initializes a new firewall instance.
* **`firewall_destroy`**: Frees the firewall instance and all associated rules/state.

### Packet Inspection
* **`firewall_check`**: The core entry point. Takes a raw packet buffer and its length. It iterates through all configured rules. If *any* rule triggers a drop, the function returns `ACTION_DROP`. If the packet is malformed (e.g., shorter than the headers imply), it returns `ACTION_DROP`. Otherwise, it returns `ACTION_PASS`.

### Rule Management
You must implement four distinct types of filtering rules:

1.  **MAC Address Filtering (`firewall_add_mac_rule`)**
    * Filters packets based on the **Source MAC Address**.
    * If the source MAC matches the rule, the specified action is taken.

2.  **Blacklisting (`firewall_add_blacklist_rule`)**
    * Filters packets based on a 5-tuple: Protocol (TCP/UDP), Source IP, Destination IP, and Destination Port range.
    * IPs are provided in **Host Byte Order**.
    * If a rule value is `0` (for IPs), it acts as a wildcard (matches any).
    * Ports are defined as a range `[start, end]`.

3.  **Deep Packet Inspection (`firewall_add_content_rule`)**
    * Searches the packet **Payload** (data after the TCP/UDP header) for an exact byte sequence.
    * This applies to both TCP and UDP packets.
    * You must correctly calculate header offsets to locate the payload.

4.  **Stateful Rate Limiting (`firewall_configure_ratelimit`)**
    * Implements a **Leaky Bucket** algorithm to rate-limit traffic flows.
    * A "flow" is identified by the unique tuple: `{SrcIP, SrcPort, DestIP, DestPort}`.
    * You must track state for every active flow.
    * The bucket fills with payload bytes and drains at `rate_bps`. If a packet arrives that would overflow the bucket, it is dropped.
    * If a flow doesn't see traffic for `timeout_sec`, it is considered inactive, and its state can be discarded.
    * Please refer to the documentation in `lib.h` for detailed behavior.

Multiple rules of the same type can be added, except for rate limiting. Rules are checked in the order they were added.

**Important**: Rules are independent of each other, and they are **ALL** evaluated for each packet. If any rule indicates to drop the packet, the final decision is to drop it, otherwise pass it.

---

## Testing Your Code

The provided test suite generates synthetic raw packets and validates your filtering logic against them. It covers endianness, header parsing, rule matching, and state management.

To run the tests:

```bash
make
./test
```

or

```bash
make run
```

You should see:

```text
* Suite suite_mac:
..........
10 tests - 10 passed, 0 failed, 0 skipped

* Suite suite_blacklist:
..........
10 tests - 10 passed, 0 failed, 0 skipped

* Suite suite_content:
..........
10 tests - 10 passed, 0 failed, 0 skipped

* Suite suite_ratelimit:
....................
20 tests - 20 passed, 0 failed, 0 skipped

Total: 50 tests, 96 assertions
```

---

## Files You'll Modify

* **`lib.c`**: Implement the `firewall_t` struct and all functions defined in `lib.h`.

## Files Provided

* **`lib.h`**: API definitions.
* **`net.h`**: Protocol struct definitions (`ethhdr_t`, `iphdr_t`, etc.).
* **`test.c`**: The unit testing suite.
* **`Makefile`**: Build instructions.
