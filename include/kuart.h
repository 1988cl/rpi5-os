#ifndef KUART_H
#define KUART_H

#include "platform.h"
#ifdef PLATFORM_QEMU_VIRT
#define UART_BASE 0x09000000UL
#elif defined(PLATFORM_RASPBERRY_PI_5)
#define UART_BASE 0x107d001000UL
#else
#error "Platform not defined! Please define PLATFORM_QEMU_VIRT or PLATFORM_RASPBERRY_PI_5"
#endif

#define UART_DR 0x00 /* Data read or written from the interface. */
#define UART_FR 0x18 /* Flag register (Read only). */

#define UART_FR_TXFF (1 << 5) /* used in ASM */

void uart_putc(char c);
void uart_puts(const char *s);

#endif
