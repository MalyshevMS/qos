#include "serial.hpp"
#include "../dev/dev.hpp"

void Serial::init() {
    Dev::outb(COM1_PORT + 1, 0x00);
    Dev::outb(COM1_PORT + 3, 0x80);
    Dev::outb(COM1_PORT + 0, 0x03);
    Dev::outb(COM1_PORT + 1, 0x00);
    Dev::outb(COM1_PORT + 3, 0x03);
    Dev::outb(COM1_PORT + 2, 0xC7);
    Dev::outb(COM1_PORT + 4, 0x0B);
}

int Serial::is_transmit_empty() {
    return Dev::inb(COM1_PORT + 5) & 0x20;
}

void Serial::write_char(char c) {
    while (!is_transmit_empty());
    Dev::outb(COM1_PORT, c);
}

void Serial::write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        write_char(str[i]);
    }
}