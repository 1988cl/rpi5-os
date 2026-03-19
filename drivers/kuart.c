#include "kuart.h"
#include "ktype.h"

static inline void mmio_write(uintptr_t reg, uint32_t val) {
    *(volatile uint32_t *)(UART_BASE + reg) = val;
}

static inline volatile uint32_t mmio_read(uintptr_t reg) {
    return *(volatile uint32_t *)(UART_BASE + reg);
}

void uart_putc(char c) {
    while (mmio_read(UART_FR) & UART_FR_TXFF)
        ;
    mmio_write(UART_DR, c);
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}
