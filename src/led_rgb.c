#include "led_rgb.h"

void init_led_rgb(uint red_pin, uint green_pin, uint blue_pin) {
    gpio_init(red_pin);
    gpio_init(green_pin);
    gpio_init(blue_pin);
    gpio_set_dir(red_pin, GPIO_OUT);
    gpio_set_dir(green_pin, GPIO_OUT);
    gpio_set_dir(blue_pin, GPIO_OUT);
}

void set_led_color(bool R, bool G, bool B) {
    gpio_put(RED_PIN, R);
    gpio_put(GREEN_PIN, G);
    gpio_put(BLUE_PIN, B);
}