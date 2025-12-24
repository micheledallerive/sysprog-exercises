#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PAGE_OFFSET_BITS 8
#define VPN_BITS 14
#define PPN_BITS 12

/**
 * In the virtual address, the lower 8 bits are the page offset,
 * and the next 14 bits are the virtual page number (VPN).
 * Any higher bit must be ignored when processing the virtual address.
 */
typedef uint32_t va_t;
typedef uint32_t pa_t;

/**
 * Page table entry.
 * Bits:
 * 0-11: Physical Page Number (PPN)
 * 12:   Valid bit
 * 13:   Read permission bit
 * 14:  Write permission bit
 * 15:  Execute permission bit
 */
typedef uint16_t page_table_entry_t;

struct page_table {
  page_table_entry_t entries[1 << VPN_BITS];
};

typedef enum { READ = 0, WRITE, EXECUTE } action_type_t;
typedef enum { SUCCESS = 0, PERMISSION_DENIED, PAGE_FAULT } result_t;

/**
 * Validate a memory access and translate virtual address to physical address.
 * If the access is valid, writes the corresponding physical address to
 * `physical_address` and returns SUCCESS.
 * If the page is not in the page table, returns PAGE_FAULT.
 * If the given action cannot be performed due to insufficient permissions,
 * returns PERMISSION_DENIED.
 */
result_t access_memory(struct page_table* pt, va_t virtual_address,
                       action_type_t action, pa_t* physical_address);

#endif  // LIB_H
