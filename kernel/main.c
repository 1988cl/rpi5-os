#include "dtb.h"
#include "kprintf.h"
extern uint64_t dtb_addr;

int main(void) {
    dtb_parser_process(dtb_addr);

    while (1)
        ;
    return 0;
}
