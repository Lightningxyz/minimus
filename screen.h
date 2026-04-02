#ifndef SCREEN_H
#define SCREEN_H

// VGA color constants
#define VGA_BLACK        0
#define VGA_BLUE         1
#define VGA_GREEN        2
#define VGA_CYAN         3
#define VGA_RED          4
#define VGA_MAGENTA      5
#define VGA_BROWN        6
#define VGA_LIGHT_GREY   7
#define VGA_DARK_GREY    8
#define VGA_LIGHT_BLUE   9
#define VGA_LIGHT_GREEN  10
#define VGA_LIGHT_CYAN   11
#define VGA_LIGHT_RED    12
#define VGA_PINK         13
#define VGA_YELLOW       14
#define VGA_WHITE        15

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

void screen_init(void);
void screen_clear(void);
void screen_putchar(char c);
void screen_print(const char *str);
void screen_print_color(const char *str, unsigned char fg, unsigned char bg);
void screen_set_color(unsigned char fg, unsigned char bg);
void screen_backspace(void);

#endif
