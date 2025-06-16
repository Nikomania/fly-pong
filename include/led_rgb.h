#ifndef LED_RGB_H
#define LED_RGB_H

#include "pico/stdlib.h"

#define RED_PIN 13
#define GREEN_PIN 11
#define BLUE_PIN 12

void init_led_rgb(uint red_pin, uint green_pin, uint blue_pin);
void set_led_color(bool R, bool G, bool B);

#endif