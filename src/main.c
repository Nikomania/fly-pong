#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "mqtt_comm.h"      // Funções personalizadas para MQTT
#include "xor_cipher.h"     // Funções de cifra XOR
#include "buzzer.h"
#include "led_rgb.h"
#include "button.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led_matrix.h"

#define DEBUG

#define WIFI_SSID ""  // Nome da rede WiFi
#define WIFI_PASSWORD "" // Senha da rede WiFi

#define MOSQUITTO_USER "1"  // Usuário para autenticação MQTT
#define MOSQUITTO_PASSWORD NULL // Senha para autenticação MQTT

#define PORT 1884
#define IP "172.19.9.43"

void vTaskLED(void *pvParameters);
void vTaskBuzzer(void *pvParameters);

int main() {
    stdio_init_all();
    sleep_ms(5000);

    pwm_init_buzzer(BUZZER_PIN);
    init_led_rgb(RED_PIN, GREEN_PIN, BLUE_PIN);

    TaskHandle_t xLEDTaskHandle;
    TaskHandle_t xBuzzerTaskHandle;

    // Cria tarefas primeiro (handles serão configurados)
    xTaskCreate(vTaskLED, "LED Task", configMINIMAL_STACK_SIZE, NULL, 2, &xLEDTaskHandle);
    xTaskCreate(vTaskBuzzer, "Buzzer Task", configMINIMAL_STACK_SIZE, NULL, 3, &xBuzzerTaskHandle);

    struct btn_state btn_a = { BTN_A, BTN_RELEASED, 0, &xLEDTaskHandle };
    struct btn_state btn_b = { BTN_B, BTN_RELEASED, 0, &xBuzzerTaskHandle };

    // Depois inicializa botões (agora com handles válidos)
    init_button(&btn_a);
    init_button(&btn_b);
    initLedMatrix();
    setLED(2, 2, WHITE); // Define o LED central como branco
    render();

    // vTaskStartScheduler();


    #ifdef DEBUG
    printf("Conectando ao WiFi...\n");
    #endif
    if (!connect_to_wifi(WIFI_SSID, WIFI_PASSWORD)) {
        while(true) {
            #ifdef DEBUG
            printf("Erro ao conectar ao WiFi!\n");
            #endif
            sleep_ms(1000); // Aguarda indefinidamente se a conexão falhar
        }
    }
    
    //connect_to_wifi("AP-ACCESS BLH", "Fin@ApointBlH");
    // Configura o cliente MQTT
    // Parâmetros: ID do cliente, IP do broker, usuário, senha
    #ifdef DEBUG
    printf("Configurando MQTT...\n");
    #endif
    if (mqtt_setup("bitdog1", IP, PORT, MOSQUITTO_USER, MOSQUITTO_PASSWORD)) {
        #ifdef DEBUG
        printf("MQTT configurado com sucesso!\n");
        #endif
    } else {
        while(true) {
            #ifdef DEBUG
            printf("Erro ao configurar MQTT!\n");
            #endif
            sleep_ms(1000); // Aguarda indefinidamente se a configuração falhar
        }
    }
    #ifdef DEBUG
    printf("MQTT configurado!\n");
    #endif

    mqtt_comm_subscribe(
        "fly-pong/room1",  // Tópico a ser assinado
        mqtt_on_request,                        // Callback de confirmação de assinatura
        mqtt_on_incoming_publish,                         // Callback de dados
        mqtt_on_message            // Callback para mensagens recebidas
    );

    while (true) {
        tight_loop_contents();
    }
}

void vTaskLED(void *pvParameters) {
    int acc = 1;
    while (true) {
        switch (acc) {
        case 1:
            set_led_color(true, false, false); // Vermelho
            break;
        case 2:
            set_led_color(false, true, false); // Verde
            break;
        case 3:
            set_led_color(false, false, true); // Azul
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        acc = (acc % 3) + 1;
    }
}

void vTaskBuzzer(void *pvParameters) {
    while (true) {
        beep(BUZZER_PIN, 100);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
