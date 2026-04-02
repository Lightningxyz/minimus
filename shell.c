#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "string.h"

#define INPUT_BUFFER_SIZE 256

// ===== I/O helpers (for RTC and reboot) =====
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ===== RTC (Real-Time Clock) helpers =====
static unsigned char read_cmos(unsigned char reg) {
    outb(0x70, reg);
    return inb(0x71);
}

// BCD to binary conversion (CMOS stores values in BCD)
static unsigned char bcd_to_bin(unsigned char bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Print a two-digit number with leading zero
static void print_two_digits(unsigned char val) {
    char buf[3];
    buf[0] = '0' + (val / 10);
    buf[1] = '0' + (val % 10);
    buf[2] = '\0';
    screen_print(buf);
}

// ===== Command: help =====
static void cmd_help(void) {
    screen_print_color("\n Available Commands:\n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print_color("  help    ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Show this help message\n");
    screen_print_color("  clear   ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Clear the screen\n");
    screen_print_color("  echo    ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Print text (usage: echo hello world)\n");
    screen_print_color("  time    ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Show current time\n");
    screen_print_color("  date    ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Show current date\n");
    screen_print_color("  about   ", VGA_YELLOW, VGA_BLACK);
    screen_print("- About this OS\n");
    screen_print_color("  reboot  ", VGA_YELLOW, VGA_BLACK);
    screen_print("- Reboot the computer\n");
    screen_print("\n");
}

// ===== Command: clear =====
static void cmd_clear(void) {
    screen_clear();
}

// ===== Command: echo =====
static void cmd_echo(const char *args) {
    if (args && *args) {
        screen_print(args);
    }
    screen_print("\n");
}

// ===== Command: time =====
static void cmd_time(void) {
    unsigned char hours = bcd_to_bin(read_cmos(0x04));
    unsigned char minutes = bcd_to_bin(read_cmos(0x02));
    unsigned char seconds = bcd_to_bin(read_cmos(0x00));

    screen_print("  Time: ");
    screen_print_color("", VGA_LIGHT_GREEN, VGA_BLACK);
    print_two_digits(hours);
    screen_print(":");
    print_two_digits(minutes);
    screen_print(":");
    print_two_digits(seconds);
    screen_print_color("", VGA_WHITE, VGA_BLACK);
    screen_print("\n");
}

// ===== Command: date =====
static void cmd_date(void) {
    unsigned char day = bcd_to_bin(read_cmos(0x07));
    unsigned char month = bcd_to_bin(read_cmos(0x08));
    unsigned char year = bcd_to_bin(read_cmos(0x09));

    screen_print("  Date: ");
    screen_print_color("", VGA_LIGHT_GREEN, VGA_BLACK);
    print_two_digits(day);
    screen_print("/");
    print_two_digits(month);
    screen_print("/20");
    print_two_digits(year);
    screen_print_color("", VGA_WHITE, VGA_BLACK);
    screen_print("\n");
}

// ===== Command: about =====
static void cmd_about(void) {
    screen_print("\n");
    screen_print_color("  Minimus OS", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print(" v0.1\n");
    screen_print("  A simple x86 operating system\n");
    screen_print("  Written in C and x86 Assembly\n");
    screen_print("  32-bit Protected Mode\n\n");
}

// ===== Command: reboot =====
static void cmd_reboot(void) {
    screen_print("Rebooting...\n");

    // Method: pulse the keyboard controller reset line
    unsigned char good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE); // Send reset command

    // If that didn't work, triple fault
    __asm__ volatile ("lidt 0");
    __asm__ volatile ("int $3");
}

// ===== Print the shell prompt =====
static void print_prompt(void) {
    screen_print_color("minimus", VGA_LIGHT_GREEN, VGA_BLACK);
    screen_print_color("> ", VGA_WHITE, VGA_BLACK);
}

// ===== Execute a command =====
static void execute_command(char *input) {
    // Skip leading spaces
    while (*input == ' ') input++;

    // Empty input
    if (*input == '\0') return;

    // Find the command (first word)
    char *args = input;
    while (*args && *args != ' ') args++;
    if (*args == ' ') {
        *args = '\0'; // Null-terminate the command
        args++;       // args points to the rest
        while (*args == ' ') args++; // Skip leading spaces in args
    }

    // Dispatch
    if (strcmp(input, "help") == 0) {
        cmd_help();
    } else if (strcmp(input, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(input, "echo") == 0) {
        cmd_echo(args);
    } else if (strcmp(input, "time") == 0) {
        cmd_time();
    } else if (strcmp(input, "date") == 0) {
        cmd_date();
    } else if (strcmp(input, "about") == 0) {
        cmd_about();
    } else if (strcmp(input, "reboot") == 0) {
        cmd_reboot();
    } else {
        screen_print_color("  Unknown command: ", VGA_LIGHT_RED, VGA_BLACK);
        screen_print(input);
        screen_print("\n  Type 'help' for available commands.\n");
    }
}

// ===== Main shell loop =====
void shell_run(void) {
    char input[INPUT_BUFFER_SIZE];
    int pos = 0;

    print_prompt();

    while (1) {
        char c = keyboard_wait();

        if (c == '\n') {
            // Enter pressed — execute command
            screen_putchar('\n');
            input[pos] = '\0';
            execute_command(input);
            pos = 0;
            print_prompt();

        } else if (c == '\b') {
            // Backspace
            if (pos > 0) {
                pos--;
                screen_backspace();
            }

        } else if (c >= 32 && c < 127) {
            // Printable character
            if (pos < INPUT_BUFFER_SIZE - 1) {
                input[pos++] = c;
                screen_putchar(c);
            }
        }
    }
}
