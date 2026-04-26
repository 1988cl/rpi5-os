#include "kstdarg.h"
#include "kuart.h"

#define UPPER 1
#define LOWER 0
const static int levelarray[10] = {1,      10,      100,      1000,      10000,
                                   100000, 1000000, 10000000, 100000000, 1000000000};
const static char *lower = "0123456789abcdef";
const static char *upper = "0123456789ABCDEF";

void int_to_char(int input, char *buffer) {

    int i = 0;
    int start = 0;
    if (input == -2147483648) {
        buffer[0] = '-';
        buffer[1] = '2';
        buffer[2] = '1';
        buffer[3] = '4';
        buffer[4] = '7';
        buffer[5] = '4';
        buffer[6] = '8';
        buffer[7] = '3';
        buffer[8] = '6';
        buffer[9] = '4';
        buffer[10] = '8';
        buffer[11] = '\0';
        return;
    }

    if (input < 0) {
        buffer[i++] = '-';
        input = -input;
    }
    if (input == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
    }

    for (int p = 9; p >= 0; p--) {

        int power = levelarray[p];
        int digit = 0;

        while (input >= power) {
            input = input - power;
            digit++;
        }

        if (digit != 0 || start == 1 || p == 0) {
            buffer[i++] = '0' + digit;
            start = 1;
        }
    }

    buffer[i] = '\0';
}

void hex_to_char(unsigned long value, char *buffer, int uppercase) {

    const char *digits = uppercase ? upper : lower;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    int i = 0;
    int start = 0;
    for (int shift = 60; shift >= 0; shift -= 4) {
        int digit = (value >> shift) & 0xf;
        if (digit != 0 || start == 1 || shift == 0) {
            buffer[i++] = digits[digit];
            start = 1;
        }
    }
    buffer[i] = '\0';
}

int kprintf(const char *format, ...) {
    va_list args;
    int chars_printed = 0;
    va_start(args, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++;
            switch (*format) {
            case 'd': {
                int value = va_arg(args, int);
                char buffer[12];
                int_to_char(value, buffer);
                char *p = buffer;
                while (*p) {
                    uart_putc(*p);
                    p++;
                    chars_printed++;
                }
                break;
            }
            case 's': {
                char *p = va_arg(args, char *);
                if (!p) {
                    p = "(null)";
                }
                while (*p) {
                    uart_putc(*p);
                    p++;
                    chars_printed++;
                }
                break;
            }
            case 'x':
            case 'X': {
                unsigned long value = va_arg(args, unsigned int);
                char buffer[70];
                hex_to_char(value, buffer, (*format == 'X'));
                char *p = buffer;
                while (*p) {
                    uart_putc(*p);
                    p++;
                    chars_printed++;
                }
                break;
            }
            case 'c': {
                char c = va_arg(args, int);
                uart_putc(c);
                chars_printed++;
                break;
            }
            case 'p': {
                void *ptr = va_arg(args, void *);
                uart_putc('0');
                uart_putc('x');
                chars_printed += 2;
                char buffer[9];
                hex_to_char((unsigned long)ptr, buffer, LOWER);
                char *p = buffer;
                while (*p) {
                    uart_putc(*p);
                    p++;
                    chars_printed++;
                }
                break;
            }
            default: {
                uart_putc('%');
                uart_putc(*format);
                chars_printed += 2;
            }
            }
        } else {
            uart_putc(*format);
            chars_printed++;
        }
        format++;
    }
    va_end(args);
    return chars_printed;
}
