#include "keyboard.h"

// I/O port helpers
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Keyboard I/O ports
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

// Scancode to ASCII lookup table (US QWERTY, lowercase)
static const char scancode_to_ascii[] = {
    0,   27,  '1', '2', '3', '4', '5', '6',  // 0x00 - 0x07
    '7', '8', '9', '0', '-', '=', '\b','\t',  // 0x08 - 0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',  // 0x10 - 0x17
    'o', 'p', '[', ']', '\n', 0,  'a', 's',   // 0x18 - 0x1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 0x20 - 0x27
    '\'','`', 0,  '\\', 'z', 'x', 'c', 'v',  // 0x28 - 0x2F
    'b', 'n', 'm', ',', '.', '/', 0,   '*',   // 0x30 - 0x37
    0,   ' ', 0,   0,   0,   0,   0,   0,     // 0x38 - 0x3F (space at 0x39)
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x40 - 0x47
    0,   0,   '-', 0,   0,   0,   '+', 0,     // 0x48 - 0x4F
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x50 - 0x57
    0,   0                                      // 0x58 - 0x59
};

// Shifted characters
static const char scancode_to_ascii_shift[] = {
    0,   27,  '!', '@', '#', '$', '%', '^',  // 0x00 - 0x07
    '&', '*', '(', ')', '_', '+', '\b','\t',  // 0x08 - 0x0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  // 0x10 - 0x17
    'O', 'P', '{', '}', '\n', 0,  'A', 'S',   // 0x18 - 0x1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  // 0x20 - 0x27
    '"', '~', 0,  '|', 'Z', 'X', 'C', 'V',   // 0x28 - 0x2F
    'B', 'N', 'M', '<', '>', '?', 0,   '*',   // 0x30 - 0x37
    0,   ' ', 0,   0,   0,   0,   0,   0,     // 0x38 - 0x3F
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x40 - 0x47
    0,   0,   '-', 0,   0,   0,   '+', 0,     // 0x48 - 0x4F
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x50 - 0x57
    0,   0                                      // 0x58 - 0x59
};

static int shift_pressed = 0;

char keyboard_read(void) {
    // Check if there's data available
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    if (!(status & 0x01)) {
        return 0; // No key available
    }

    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    // Track shift key state
    // Left shift press = 0x2A, Right shift press = 0x36
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }
    // Left shift release = 0xAA, Right shift release = 0xB6
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return 0;
    }

    // Ignore key releases (bit 7 set)
    if (scancode & 0x80) {
        return 0;
    }

    // Convert scancode to ASCII
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            return scancode_to_ascii_shift[scancode];
        }
        return scancode_to_ascii[scancode];
    }

    return 0;
}

char keyboard_wait(void) {
    char c;
    while (1) {
        c = keyboard_read();
        if (c != 0) return c;
    }
}
