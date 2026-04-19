#include <kernel/serial.hpp>
#include <kernel/ports.hpp>

namespace Kernel::Serial {
    
using namespace Ports;
using namespace kstd;

void init() {
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x80);
    outb(COM1_PORT + 0, 0x03);
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xC7);
    outb(COM1_PORT + 4, 0x0B);
}

int is_transmit_empty() {
    return inb(COM1_PORT + 5) & 0x20;
}

void write_char(char c) {
    while (!is_transmit_empty());
    outb(COM1_PORT, c);
}

void write(const char* str) {
    for (int i = 0; str[i] != 0; i++) {
        write_char(str[i]);
    }
}

void write_hex(size_t hex) {
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

void print(const string& text) {
    write(text.c_str());
}

void println(const string& text = "") {
    print(text);
    write_char('\n');
}

} // namespace Kernel::Serial