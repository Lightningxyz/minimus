#include "screen.h"
#include "shell.h"

void kernel_main(void) {
    // Initialize the screen
    screen_init();

    // Print boot banner
    screen_print_color("  __  __ _       _                     \n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print_color(" |  \\/  (_)_ __ (_)_ __ ___  _   _ ___ \n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print_color(" | |\\/| | | '_ \\| | '_ ` _ \\| | | / __|\n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print_color(" | |  | | | | | | | | | | | | |_| \\__ \\\n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print_color(" |_|  |_|_|_| |_|_|_| |_| |_|\\__,_|___/\n", VGA_LIGHT_CYAN, VGA_BLACK);
    screen_print("\n");

    screen_print_color("  Welcome to Minimus OS v0.1\n", VGA_WHITE, VGA_BLACK);
    screen_print_color("  Type 'help' for available commands.\n\n", VGA_DARK_GREY, VGA_BLACK);

    // Launch the shell
    shell_run();
}