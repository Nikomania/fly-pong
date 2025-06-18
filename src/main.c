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
#include "ball.h"

#define DEBUG

#define WIFI_SSID ""  // Nome da rede WiFi
#define WIFI_PASSWORD "" // Senha da rede WiFi

#define MOSQUITTO_USER "fly-pong"  // Usuário para autenticação MQTT
#define MOSQUITTO_PASSWORD NULL // Senha para autenticação MQTT

#define PORT 1883
#define IP "192.168.131.35"

void game_task(void *pvParameters);
void wifi_conn_task(void *pvParameters);

int main() {
    stdio_init_all();
    sleep_ms(5000);

    init_button(BTN_A);
    init_button(BTN_B);
    side = FLY;
    ball_init(side);
    initLedMatrix();

    xTaskCreate(
        wifi_conn_task,          // Função da tarefa
        "WiFi Connection Task",  // Nome da tarefa
        configMINIMAL_STACK_SIZE, // Tamanho da pilha
        NULL,                    // Parâmetros da tarefa
        1,                       // Prioridade da tarefa
        NULL                     // Handle da tarefa (não usado aqui)
    );

    xTaskCreate(
        game_task,               // Função da tarefa
        "Game Task",             // Nome da tarefa
        configMINIMAL_STACK_SIZE, // Tamanho da pilha
        NULL,                    // Parâmetros da tarefa
        1,                       // Prioridade da tarefa
        NULL                     // Handle da tarefa (não usado aqui)
    );

    vTaskStartScheduler();

    while (true) {
        tight_loop_contents();
    }
}

void game_task(void *pvParameters) {
    while (true) {
        game_tick(); // Atualiza o estado do jogo
        game_render(); // Renderiza o estado do jogo no LED Matrix
        vTaskDelay(pdMS_TO_TICKS(TICK_DELAY)); // Aguarda um tempo antes da próxima iteração
    }
}

void wifi_conn_task(void *pvParameters) {
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

    while(true) {
        if (mqtt_has_new_data()) {
            // Se houver novos dados, processa a mensagem recebida
            const char *topic = mqtt_get_last_topic();
            float value = mqtt_get_last_value();
            unsigned long int timestamp = mqtt_get_last_timestamp();

            #ifdef DEBUG
            printf("Mensagem recebida no tópico '%s': valor=%.2f, timestamp=%lu\n", topic, value, timestamp);
            #endif

            // Aqui você pode adicionar lógica para processar a mensagem recebida
            // Por exemplo, atualizar o estado do jogo ou enviar uma resposta
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo antes da próxima iteração
    }
}
