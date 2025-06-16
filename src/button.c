#include "button.h"
#include "led_rgb.h"
#include "buzzer.h"

void init_button(struct btn_state* btn) {
    gpio_init(btn->pin);
    gpio_set_dir(btn->pin, GPIO_IN);
    gpio_pull_up(btn->pin);
    const char* task_name = (btn->pin == BTN_A) ? "Button A Task" : "Button B Task";
    xTaskCreate(vTaskButton, task_name, configMINIMAL_STACK_SIZE, btn, 1, NULL);
}

void vTaskButton(void *pvParameters) {
    struct btn_state *btn = (struct btn_state *)pvParameters;
    ButtonState new_state;
    
    while (true) {
        bool current_pin_state = gpio_get(btn->pin) == 0;
        new_state = current_pin_state ? BTN_PRESSED : BTN_RELEASED;
        
        if (new_state != btn->state) {
            uint32_t now = to_ms_since_boot(get_absolute_time());
            if (now - btn->last_change_time > BTN_DEBOUNCE) {
                btn->state = new_state;
                btn->last_change_time = now;
                
                // Ação baseada no novo estado
                if (*(btn->task_handle) != NULL) {
                    if (btn->state == BTN_PRESSED) {
                        vTaskSuspend(*(btn->task_handle)); // Suspende ao pressionar
                    } else {
                        vTaskResume(*(btn->task_handle));  // Retoma ao soltar
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE)); // Polling a cada 100ms
    }
}