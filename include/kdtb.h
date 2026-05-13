#ifndef KDTB_H
#define KDTB_H
#include "ktype.h"

#define RSV_SIZE 16
#define FDT_BEGIN_NODE (0x00000001)
#define FDT_END_NODE (0x00000002)
#define FDT_PROP (0x00000003)
#define FDT_NOP (0x00000004)
#define FDT_END (0x00000009)

typedef enum { NODE_NONE, NODE_MEMORY } node_type_t;
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

struct dtb_fdt_prop {
    uint32_t len;
    uint32_t nameoff;
};

void dtb_parser_process(uint64_t dtb_addr);
struct dtb_header *dtb_parser_init(uint64_t dtb_addr);
uint8_t dtb_parser_reserved_memory(uint64_t reserved_memory_address);
void dtb_parser_struct(uint64_t dtb_struct_addr, uint64_t dtb_strings_addr);
uint32_t dtb_reverse_byte_32(uint32_t value);
uint64_t dtb_reverse_byte_64(uint64_t value);
#endif
