#include "dtb.h"
#include "kprintf.h"

struct dtb_reserve_entry rsv_memory_array[RSV_SIZE];

void dtb_parser_process(uint64_t dtb_addr) {
    struct dtb_header *header = dtb_parser_init(dtb_addr);
    if (header == NULL) {
        kprintf("the dtb header is null");
        return;
    }
    uint64_t rsv_memory_addr = dtb_addr + (uint64_t)(dtb_reverse_byte_32(header->off_mem_rsvmap));
    dtb_parser_reserved_memory(rsv_memory_addr);
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
