#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2818b.pio.h"

#define LED_COUNT_X 5
#define LED_COUNT_Y 5
#define LED_COUNT (LED_X_COUNT * LED_Y_COUNT)
#define LED_MATRIX_PIN 7

#define RGB_INTENSITY_LEVEL 5
#define RGB_INTENSITY 1 << RGB_INTENSITY_LEVEL

typedef struct rgb_t {
  uint8_t R, G, B;
} rgb_t;

typedef enum COLORS {
  BLACK,
  WHITE,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  CYAN,
  MAGENTA,
  COLORS_COUNT
} COLORS;

void initLedMatrix();

void setLED(const uint index_x, const uint index_y, const COLORS color);
void setLEDs(const COLORS leds[LED_COUNT_X][LED_COUNT_Y]);
void clearLEDs();
void renderLedMatrix();
void clear();

void setSmile(COLORS color);
void setCheck(COLORS color);
void setCross(COLORS color);

void setNumber(const uint8_t number, COLORS color);
void setNumberOne(COLORS color);
void setNumberTwo(COLORS color);
void setNumberThree(COLORS color);
void setNumberFour(COLORS color);
void setNumberFive(COLORS color);
void setNumberSix(COLORS color);
void setNumberSeven(COLORS color);
void setNumberEight(COLORS color);
void setNumberNine(COLORS color);
void setNumberZero(COLORS color);


#endif // LED_MATRIX_H
