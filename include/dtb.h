#ifndef DTB_H
#define DTB_H
#include "ktype.h"

#define RSV_SIZE 16

struct dtb_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};
struct dtb_reserve_entry {
    uint64_t address;
    uint64_t size;
};

void dtb_parser_process(uint64_t dtb_addr);
struct dtb_header *dtb_parser_init(uint64_t dtb_addr);
uint8_t dtb_parser_reserved_memory(uint64_t reserved_memory_address);
uint32_t dtb_reverse_byte_32(uint32_t value);
uint64_t dtb_reverse_byte_64(uint64_t value);
#endif
