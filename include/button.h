#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h"

#define BTN_A 5
#define BTN_B 6
#define BTN_DEBOUNCE 50 // 50ms para debounce

// Estados do botão
typedef enum {
    BTN_RELEASED = 0,
    BTN_PRESSED = 1
} ButtonState;

// Protótipos
void init_button(int pin);
ButtonState read_button(int pin);

#endif