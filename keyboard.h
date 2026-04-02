#ifndef KEYBOARD_H
#define KEYBOARD_H

// Polls the keyboard controller and returns an ASCII character.
// Returns 0 if no key is available.
char keyboard_read(void);

// Blocks until a key is pressed and returns the ASCII character.
char keyboard_wait(void);

#endif
