#include "screen.h"
#include "string.h"

// VGA text mode memory starts at 0xB8000
static unsigned short *vga_buffer = (unsigned short *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char current_color = 0x0F; // White on black

// I/O port helpers (inline assembly)
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Make a VGA entry: character + color attribute
static unsigned short vga_entry(char c, unsigned char color) {
    return (unsigned short)c | ((unsigned short)color << 8);
}

// Update the hardware cursor position
static void update_cursor(void) {
    unsigned short pos = cursor_y * SCREEN_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

// Scroll the screen up by one line
static void scroll(void) {
    // Move everything up one row
    for (int i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + SCREEN_WIDTH];
    }
    // Clear the last row
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        vga_buffer[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + i] = vga_entry(' ', current_color);
    }
}

void screen_init(void) {
    current_color = 0x0F; // White on black
    screen_clear();
}

void screen_clear(void) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga_buffer[i] = vga_entry(' ', current_color);
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void screen_set_color(unsigned char fg, unsigned char bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}

void screen_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        // Tab to next 4-column boundary
        cursor_x = (cursor_x + 4) & ~3;
    } else {
        vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = vga_entry(c, current_color);
        cursor_x++;
    }

    // Wrap to next line
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll if we're past the bottom
    if (cursor_y >= SCREEN_HEIGHT) {
        scroll();
        cursor_y = SCREEN_HEIGHT - 1;
    }

    update_cursor();
}

void screen_print(const char *str) {
    while (*str) {
        screen_putchar(*str++);
    }
}

void screen_print_color(const char *str, unsigned char fg, unsigned char bg) {
    unsigned char old_color = current_color;
    screen_set_color(fg, bg);
    screen_print(str);
    current_color = old_color;
}

void screen_backspace(void) {
    if (cursor_x > 0) {
        cursor_x--;
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = SCREEN_WIDTH - 1;
    }
    vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = vga_entry(' ', current_color);
    update_cursor();
}
