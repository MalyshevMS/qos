#include <driver/keyboard.hpp>
#include <kernel/ports.hpp>
#include <cfg/asm.txx>

using namespace Arch;
using namespace Kernel;
using namespace Ports;

namespace Driver::Keyboard {

// Global state
static uint8_t last_scancode = 0;
static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;
static bool capslock_active = false;

// Conversion table from scancode to ASCII character (without modifiers)
static const char scancode_to_char[] = {
    0,      // 0x00 - invalid
    0,      // 0x01 - ESC
    '1',    // 0x02
    '2',    // 0x03
    '3',    // 0x04
    '4',    // 0x05
    '5',    // 0x06
    '6',    // 0x07
    '7',    // 0x08
    '8',    // 0x09
    '9',    // 0x0A
    '0',    // 0x0B
    '-',    // 0x0C
    '=',    // 0x0D
    '\b',   // 0x0E - BACKSPACE
    '\t',   // 0x0F - TAB
    'q',    // 0x10
    'w',    // 0x11
    'e',    // 0x12
    'r',    // 0x13
    't',    // 0x14
    'y',    // 0x15
    'u',    // 0x16
    'i',    // 0x17
    'o',    // 0x18
    'p',    // 0x19
    '[',    // 0x1A
    ']',    // 0x1B
    '\n',   // 0x1C - ENTER
    0,      // 0x1D - LCTRL
    'a',    // 0x1E
    's',    // 0x1F
    'd',    // 0x20
    'f',    // 0x21
    'g',    // 0x22
    'h',    // 0x23
    'j',    // 0x24
    'k',    // 0x25
    'l',    // 0x26
    ';',    // 0x27
    '\'',   // 0x28
    '`',    // 0x29
    0,      // 0x2A - LSHIFT
    '\\',   // 0x2B
    'z',    // 0x2C
    'x',    // 0x2D
    'c',    // 0x2E
    'v',    // 0x2F
    'b',    // 0x30
    'n',    // 0x31
    'm',    // 0x32
    ',',    // 0x33
    '.',    // 0x34
    '/',    // 0x35
    0,      // 0x36 - RSHIFT
    '*',    // 0x37 - NUMPAD *
    0,      // 0x38 - LALT
    ' ',    // 0x39 - SPACE
    0,      // 0x3A - CAPSLOCK
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - NUMLOCK
    0,      // 0x46 - SCROLLLOCK
};

static const char scancode_to_char_shift[] = {
    0,      // 0x00
    0,      // 0x01 - ESC
    '!',    // 0x02 - Shift+1
    '@',    // 0x03 - Shift+2
    '#',    // 0x04 - Shift+3
    '$',    // 0x05 - Shift+4
    '%',    // 0x06 - Shift+5
    '^',    // 0x07 - Shift+6
    '&',    // 0x08 - Shift+7
    '*',    // 0x09 - Shift+8
    '(',    // 0x0A - Shift+9
    ')',    // 0x0B - Shift+0
    '_',    // 0x0C - Shift+-
    '+',    // 0x0D - Shift+=
    '\b',   // 0x0E - BACKSPACE
    '\t',   // 0x0F - TAB
    'Q',    // 0x10
    'W',    // 0x11
    'E',    // 0x12
    'R',    // 0x13
    'T',    // 0x14
    'Y',    // 0x15
    'U',    // 0x16
    'I',    // 0x17
    'O',    // 0x18
    'P',    // 0x19
    '{',    // 0x1A - Shift+[
    '}',    // 0x1B - Shift+]
    '\n',   // 0x1C - ENTER
    0,      // 0x1D - LCTRL
    'A',    // 0x1E
    'S',    // 0x1F
    'D',    // 0x20
    'F',    // 0x21
    'G',    // 0x22
    'H',    // 0x23
    'J',    // 0x24
    'K',    // 0x25
    'L',    // 0x26
    ':',    // 0x27 - Shift+;
    '"',    // 0x28 - Shift+'
    '~',    // 0x29 - Shift+`
    0,      // 0x2A - LSHIFT
    '|',    // 0x2B - Shift+Backslash
    'Z',    // 0x2C
    'X',    // 0x2D
    'C',    // 0x2E
    'V',    // 0x2F
    'B',    // 0x30
    'N',    // 0x31
    'M',    // 0x32
    '<',    // 0x33 - Shift+,
    '>',    // 0x34 - Shift+.
    '?',    // 0x35 - Shift+/
    0,      // 0x36 - RSHIFT
    '*',    // 0x37 - NUMPAD *
    0,      // 0x38 - LALT
    ' ',    // 0x39 - SPACE
    0,      // 0x3A - CAPSLOCK
    0,      // 0x3B - F1
    0,      // 0x3C - F2
    0,      // 0x3D - F3
    0,      // 0x3E - F4
    0,      // 0x3F - F5
    0,      // 0x40 - F6
    0,      // 0x41 - F7
    0,      // 0x42 - F8
    0,      // 0x43 - F9
    0,      // 0x44 - F10
    0,      // 0x45 - NUMLOCK
    0,      // 0x46 - SCROLLLOCK
};

void keyboard_callback(const x86::Registers* regs) {
    uint8_t scancode = inb(0x60);
    
    if (scancode & 0x80) {
        uint8_t released_key = scancode & 0x7F;
        
        if (released_key == SCANCODE_LSHIFT || released_key == SCANCODE_RSHIFT) {
            shift_pressed = false;
        } else if (released_key == SCANCODE_LCTRL || released_key == SCANCODE_RCTRL) {
            ctrl_pressed = false;
        } else if (released_key == SCANCODE_LALT || released_key == SCANCODE_RALT) {
            alt_pressed = false;
        }
    } else {
        // Key press
        last_scancode = scancode;
        
        // Handle modifier key presses
        if (scancode == SCANCODE_LSHIFT || scancode == SCANCODE_RSHIFT) {
            shift_pressed = true;
        } else if (scancode == SCANCODE_LCTRL || scancode == SCANCODE_RCTRL) {
            ctrl_pressed = true;
        } else if (scancode == SCANCODE_LALT || scancode == SCANCODE_RALT) {
            alt_pressed = true;
        } else if (scancode == SCANCODE_CAPSLOCK) {
            capslock_active = !capslock_active;
        }
    }
    
    EOI_MASTER;
}

void init() {
    last_scancode = 0;
    shift_pressed = false;
    ctrl_pressed = false;
    alt_pressed = false;
    capslock_active = false;
}

uint8_t getscan() {
    uint8_t code = last_scancode;
    last_scancode = 0;
    return code;
}

char scantochar(uint8_t scancode) {
    // Check bounds
    if (scancode >= sizeof(scancode_to_char)) {
        return 0;
    }
    
    // Determine which conversion table to use
    const char *table = scancode_to_char;
    
    // For letters, handle Shift+CapsLock
    if (scancode >= 0x10 && scancode <= 0x19) {  // q-p row
        if (scancode >= 0x10 && scancode <= 0x19) { // QWERTY letters
            if ((shift_pressed && !capslock_active) || (!shift_pressed && capslock_active)) {
                table = scancode_to_char_shift;
            }
        }
    } else if (scancode >= 0x1E && scancode <= 0x26) {  // a-l row
        if ((shift_pressed && !capslock_active) || (!shift_pressed && capslock_active)) {
            table = scancode_to_char_shift;
        }
    } else if (scancode >= 0x2C && scancode <= 0x32) {  // z-m row
        if ((shift_pressed && !capslock_active) || (!shift_pressed && capslock_active)) {
            table = scancode_to_char_shift;
        }
    } else if (shift_pressed) {
        // For non-letter keys, use shift table if Shift is pressed
        table = scancode_to_char_shift;
    }
    
    char ch = table[scancode];
    
    return ch;
}

// Please, try to avoid using this function. Instead use scantochar + getscan
char getchar() {
    if (last_scancode == 0) {
        return 0;
    }
    
    uint8_t scancode = last_scancode;
    last_scancode = 0;
    
    return scantochar(scancode);
}

bool has_data() {
    return last_scancode != 0;
}

void flush() {
    last_scancode = 0;
    shift_pressed = false;
    ctrl_pressed = false;
    alt_pressed = false;
    capslock_active = false;
}

bool is_shift_pressed() {
    return shift_pressed;
}

bool is_ctrl_pressed() {
    return ctrl_pressed;
}

bool is_alt_pressed() {
    return alt_pressed;
}

} // namespace Keyboard
