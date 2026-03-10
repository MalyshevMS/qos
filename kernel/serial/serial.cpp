#include <kernel/serial.hpp>
#include <kernel/ports.hpp>

using namespace Kernel;

void Serial::init() {
    Ports::outb(COM1_PORT + 1, 0x00);
    Ports::outb(COM1_PORT + 3, 0x80);
    Ports::outb(COM1_PORT + 0, 0x03);
    Ports::outb(COM1_PORT + 1, 0x00);
    Ports::outb(COM1_PORT + 3, 0x03);
    Ports::outb(COM1_PORT + 2, 0xC7);
    Ports::outb(COM1_PORT + 4, 0x0B);
}

int Serial::is_transmit_empty() {
    return Ports::inb(COM1_PORT + 5) & 0x20;
}

void Serial::write_char(char c) {
    while (!is_transmit_empty());
    Ports::outb(COM1_PORT, c);
}

void Serial::write(const char* str) {
    for (int i = 0; str[i] != 0; i++) {
        write_char(str[i]);
    }
}

void Serial::write_hex(size_t hex) {
    char buffer[11];
    const char* hex_chars = "0123456789ABCDEF";

    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = 0;

    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_chars[hex & 0xF];
        hex >>= 4;
    }

    write(buffer);
}