#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define BTN_A 5
#define BTN_B 6
#define BTN_DEBOUNCE 50 // 50ms para debounce

// Estados do botão
typedef enum {
    BTN_RELEASED,
    BTN_PRESSED
} ButtonState;

// Estrutura de estado do botão
struct btn_state {
    uint pin;
    ButtonState state;
    uint32_t last_change_time;
    TaskHandle_t *task_handle; // Handle da tarefa a ser controlada
};

// Protótipos
void init_button(struct btn_state* btn);
void vTaskButton(void *pvParameters);

#endif