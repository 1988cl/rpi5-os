#include "kdtb.h"
#include "kmm.h"
#include "kprintf.h"
#include "kstring.h"
struct dtb_reserve_entry rsv_memory_array[RSV_SIZE];
uint32_t address_cell = 2;
uint32_t size_cell = 1;
static node_type_t current_node = NODE_NONE;

void dtb_parser_process(uint64_t dtb_addr) {
    struct dtb_header *header = dtb_parser_init(dtb_addr);
    if (header == NULL) {
        kprintf("the dtb header is null");
        return;
    }
    uint64_t rsv_memory_addr = dtb_addr + (uint64_t)(dtb_reverse_byte_32(header->off_mem_rsvmap));
    dtb_parser_reserved_memory(rsv_memory_addr);
    uint64_t dtb_struct_addr = dtb_addr + (uint64_t)(dtb_reverse_byte_32(header->off_dt_struct));
    uint64_t dtb_strings_addr = dtb_addr + (uint64_t)(dtb_reverse_byte_32(header->off_dt_strings));
    dtb_parser_struct(dtb_struct_addr, dtb_strings_addr);
}

struct dtb_header *dtb_parser_init(uint64_t dtb_addr) {

    struct dtb_header *address = (struct dtb_header *)dtb_addr;
    if (address == NULL) {
        kprintf("the address is null");
        return NULL;
    }

    uint32_t magicNumber = dtb_reverse_byte_32(address->magic);
    if (magicNumber != 0xd00dfeed) {
        kprintf("the dtb magic number is %x, which is not right\n", magicNumber);
        return NULL;
    }

    uint32_t totalSize = dtb_reverse_byte_32(address->totalsize);
    uint32_t off_dt_struct = dtb_reverse_byte_32(address->off_dt_struct);
    uint32_t off_dt_strings = dtb_reverse_byte_32(address->off_dt_strings);
    uint32_t off_mem_rsvmap = dtb_reverse_byte_32(address->off_mem_rsvmap);
    uint32_t version = dtb_reverse_byte_32(address->version);
    uint32_t last_comp_version = dtb_reverse_byte_32(address->last_comp_version);
    uint32_t boot_cpuid_phys = dtb_reverse_byte_32(address->boot_cpuid_phys);
    uint32_t size_dt_strings = dtb_reverse_byte_32(address->size_dt_strings);
    uint32_t size_dt_struct = dtb_reverse_byte_32(address->size_dt_struct);

    kprintf("the dtb address is %x\n", dtb_addr);
    kprintf("the Magic number:%x\n", magicNumber);
    kprintf("the totalSize:%x\n", totalSize);
    kprintf("the off_dt_struct:%x\n", off_dt_struct);
    kprintf("the off_dt_strings:%x\n", off_dt_strings);
    kprintf("the off_mem_rsvmap:%x\n", off_mem_rsvmap);
    kprintf("the version:%x\n", version);
    kprintf("the last_comp_version:%x\n", last_comp_version);
    kprintf("the boot_cpuid_phys:%x\n", boot_cpuid_phys);
    kprintf("the size_dt_strings:%x\n", size_dt_strings);
    kprintf("the size_dt_struct:%x\n", size_dt_struct);

    return address;
}

uint8_t dtb_parser_reserved_memory(uint64_t reserved_memory_address) {
    struct dtb_reserve_entry *rsv_addr = (struct dtb_reserve_entry *)reserved_memory_address;
    uint8_t rsv_counter = 0;
    uint64_t address = 0;
    uint64_t size = 0;
    uint8_t result = FALSE;
    while (TRUE) {
        address = rsv_addr->address;
        size = rsv_addr->size;
        rsv_memory_array[rsv_counter].address = dtb_reverse_byte_64(address);
        rsv_memory_array[rsv_counter].size = dtb_reverse_byte_64(size);

        if (address == 0 && size == 0) {
            result = TRUE;
            break;
        }
        if (rsv_counter >= RSV_SIZE) {
            break;
        }
        rsv_counter++;
        rsv_addr++;
    }
    return result;
}

void dtb_parser_struct(uint64_t dtb_struct_addr, uint64_t dtb_strings_addr) {
    uint32_t *struct_addr_p = (uint32_t *)dtb_struct_addr;
    char *strings_addr_p = (char *)dtb_strings_addr;
    while (TRUE) {
        uint32_t token = dtb_reverse_byte_32(*struct_addr_p);
        struct_addr_p++;

        switch (token) {
        case FDT_BEGIN_NODE: {
            char *node_name = (char *)struct_addr_p;
            uint32_t name_len = k_strlen(node_name) + 1;
            kprintf("the node_name is : %s\n", node_name);
            if (k_str_startwith(node_name, "memory")) {
                current_node = NODE_MEMORY;
            }
            struct_addr_p += (name_len + 3) / 4;
            break;
        }
        case FDT_END_NODE: {
            current_node = NODE_NONE;
            break;
        }
        case FDT_PROP: {
            struct dtb_fdt_prop *p_prop = (struct dtb_fdt_prop *)struct_addr_p;
            uint32_t len = dtb_reverse_byte_32(p_prop->len);
            uint32_t name_off = dtb_reverse_byte_32(p_prop->nameoff);
            char *prop_name = strings_addr_p + name_off;
            struct_addr_p += sizeof(struct dtb_fdt_prop) / 4;
            // kprintf("the prop name is:%s,the data len is %d\n", prop_name, len);

            if (k_strcmp(prop_name, "#address-cells") == 0) {
                address_cell = dtb_reverse_byte_32(*struct_addr_p);
                kprintf("the #address-cells value is %d\n", address_cell);
            }
            if (k_strcmp(prop_name, "#size-cells") == 0) {
                size_cell = dtb_reverse_byte_32(*struct_addr_p);
                kprintf("the size_cell value is %d\n", size_cell);
            }
            if (current_node == NODE_MEMORY && !k_strcmp(prop_name, "reg")) {
                if (address_cell == 1) {
                    mm_addr = (uint64_t)(dtb_reverse_byte_32(struct_addr_p[0]));
                } else if (address_cell == 2) {
                    mm_addr = ((uint64_t)(dtb_reverse_byte_32(struct_addr_p[0]))) << 32;
                    mm_addr |= dtb_reverse_byte_32(struct_addr_p[1]);
                }
                if (size_cell == 1) {
                    mm_size = (uint64_t)(dtb_reverse_byte_32(struct_addr_p[address_cell]));
                } else if (size_cell == 2) {
                    mm_size = ((uint64_t)(dtb_reverse_byte_32(struct_addr_p[address_cell]))) << 32;
                    mm_size |= dtb_reverse_byte_32(struct_addr_p[address_cell + 1]);
                }
                kprintf("the memory address is %x, the size is %x \n", mm_addr, mm_size);
            }
            struct_addr_p += (len + 3) / 4;
            break;
        }
        case FDT_END: {
            kprintf("the dtb parser done!");
            return;
        }
        case FDT_NOP: {
            break;
        }
        default:
            break;
        }
    }
}

uint32_t dtb_reverse_byte_32(uint32_t value) {
    return ((value << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
            (value >> 24));
}

uint64_t dtb_reverse_byte_64(uint64_t value) {
    return ((value << 56) | ((value & 0x000000000000FF00) << 40) |
            ((value & 0x0000000000FF0000) << 24) | ((value & 0x00000000FF000000) << 8) |
            ((value & 0x000000FF00000000) >> 8) | ((value & 0x0000FF0000000000) >> 24) |
            ((value & 0x00FF000000000000) >> 40) | (value >> 56));
}
