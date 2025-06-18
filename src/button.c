#include "button.h"

void init_button(int pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

ButtonState read_button(int pin) {
    if (gpio_get(pin) == 0) {
        return BTN_PRESSED;
    } else {
        return BTN_RELEASED;
    }
}