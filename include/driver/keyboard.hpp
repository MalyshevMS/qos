#pragma once

#include <stdint.h>
#include <arch/x86/idt.hpp>

namespace Kernel {
namespace Keyboard {
    // Scancode macros for US keyboard layout
    // Numbers
    constexpr int SCANCODE_1 = 0x02;
    constexpr int SCANCODE_2 = 0x03;
    constexpr int SCANCODE_3 = 0x04;
    constexpr int SCANCODE_4 = 0x05;
    constexpr int SCANCODE_5 = 0x06;
    constexpr int SCANCODE_6 = 0x07;
    constexpr int SCANCODE_7 = 0x08;
    constexpr int SCANCODE_8 = 0x09;
    constexpr int SCANCODE_9 = 0x0A;
    constexpr int SCANCODE_0 = 0x0B;

    // Letters (lowercase by default, uppercase with Shift)
    constexpr int SCANCODE_A = 0x1E;
    constexpr int SCANCODE_B = 0x30;
    constexpr int SCANCODE_C = 0x2E;
    constexpr int SCANCODE_D = 0x20;
    constexpr int SCANCODE_E = 0x12;
    constexpr int SCANCODE_F = 0x21;
    constexpr int SCANCODE_G = 0x22;
    constexpr int SCANCODE_H = 0x23;
    constexpr int SCANCODE_I = 0x17;
    constexpr int SCANCODE_J = 0x24;
    constexpr int SCANCODE_K = 0x25;
    constexpr int SCANCODE_L = 0x26;
    constexpr int SCANCODE_M = 0x32;
    constexpr int SCANCODE_N = 0x31;
    constexpr int SCANCODE_O = 0x18;
    constexpr int SCANCODE_P = 0x19;
    constexpr int SCANCODE_Q = 0x10;
    constexpr int SCANCODE_R = 0x13;
    constexpr int SCANCODE_S = 0x1F;
    constexpr int SCANCODE_T = 0x14;
    constexpr int SCANCODE_U = 0x16;
    constexpr int SCANCODE_V = 0x2F;
    constexpr int SCANCODE_W = 0x11;
    constexpr int SCANCODE_X = 0x2D;
    constexpr int SCANCODE_Y = 0x15;
    constexpr int SCANCODE_Z = 0x2C;

    // Special characters
    constexpr int SCANCODE_SPACE = 0x39;
    constexpr int SCANCODE_ENTER = 0x1C;
    constexpr int SCANCODE_TAB = 0x0F;
    constexpr int SCANCODE_MINUS = 0x0C;      // -_
    constexpr int SCANCODE_EQUAL = 0x0D;      // =+
    constexpr int SCANCODE_LBRACE = 0x1A;      // [{
    constexpr int SCANCODE_RBRACE = 0x1B;      // ]}
    constexpr int SCANCODE_SEMI = 0x27;      // ;:
    constexpr int SCANCODE_QUOTE = 0x28;      // '"
    constexpr int SCANCODE_COMMA = 0x33;      // ,<
    constexpr int SCANCODE_PERIOD = 0x34;      // .>
    constexpr int SCANCODE_SLASH = 0x35;      // /?
    constexpr int SCANCODE_BSLASH = 0x2B;      // \|
    constexpr int SCANCODE_GRAVE = 0x29;      // `~

    // Control keys
    constexpr int SCANCODE_BACKSPACE = 0x0E;
    constexpr int SCANCODE_LSHIFT = 0x2A;
    constexpr int SCANCODE_RSHIFT = 0x36;
    constexpr int SCANCODE_LCTRL = 0x1D;
    constexpr int SCANCODE_RCTRL = 0x9D;
    constexpr int SCANCODE_LALT = 0x38;
    constexpr int SCANCODE_RALT = 0xB8;
    constexpr int SCANCODE_CAPSLOCK = 0x3A;
    constexpr int SCANCODE_ESC = 0x01;

    // Arrow keys
    constexpr int SCANCODE_UP = 0x48;
    constexpr int SCANCODE_DOWN = 0x50;
    constexpr int SCANCODE_LEFT = 0x4B;
    constexpr int SCANCODE_RIGHT = 0x4D;

    // Function keys
    constexpr int SCANCODE_F1 = 0x3B;
    constexpr int SCANCODE_F2 = 0x3C;
    constexpr int SCANCODE_F3 = 0x3D;
    constexpr int SCANCODE_F4 = 0x3E;
    constexpr int SCANCODE_F5 = 0x3F;
    constexpr int SCANCODE_F6 = 0x40;
    constexpr int SCANCODE_F7 = 0x41;
    constexpr int SCANCODE_F8 = 0x42;
    constexpr int SCANCODE_F9 = 0x43;
    constexpr int SCANCODE_F10 = 0x44;
    constexpr int SCANCODE_F11 = 0x57;
    constexpr int SCANCODE_F12 = 0x58;

    // Delete and special
    constexpr int SCANCODE_DEL = 0x53;
    constexpr int SCANCODE_HOME = 0x47;
    constexpr int SCANCODE_END = 0x4F;
    constexpr int SCANCODE_PGUP = 0x49;
    constexpr int SCANCODE_PGDN = 0x51;

    // Initialize keyboard drivercode
    void init();

    // Get last pressed key scancode
    uint8_t getscan();

    // Get character from keyboard (for printable ASCII characters)
    // Returns the character if it's a printable key, 0 if no key or non-printable key
    char getchar();

    // Check if key buffer has data
    bool has_data();

    // Clear key buffer
    void flush();

    char scantochar(uint8_t scancode);

    void keyboard_callback(const Arch::x86::Registers* regs);

} // namespace Keyboard
} // namespace Kernel
