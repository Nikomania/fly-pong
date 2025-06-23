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

#define MY_SIDE_FLY 1

#ifdef MY_SIDE_FLY
#define MY_SIDE FLY
#define MQTT_NAME "fly"
#define MQTT_SEND_ROOM "fly-pong/room1/fly"
#define MQTT_RECEIVE_ROOM "fly-pong/room1/pong"
#elif MY_SIDE_PONG
#define MY_SIDE PONG
#define MQTT_NAME "pong"
#define MQTT_SEND_ROOM "fly-pong/room1/pong"
#define MQTT_RECEIVE_ROOM "fly-pong/room1/fly"
#endif

volatile bool connected = false; // Flag para verificar se a conexão foi estabelecida

void game_task(void *pvParameters);
void wifi_conn_task(void *pvParameters);

int main() {
    stdio_init_all();
    sleep_ms(5000);

    init_button(BTN_A);
    init_button(BTN_B);
    side = MY_SIDE;
    ball_init(INITIAL_BALL_SIDE);
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
    int i = 0;
    while (!connected) {
        int sequence[4][2] = {{1, 2}, {2, 3}, {3, 2}, {2, 1}};
        clear();
        setLED(sequence[i][0], sequence[i][1], BLUE);
        if (++i > 3) i=0;
        vTaskDelay(pdMS_TO_TICKS(TICK_DELAY)); // Aguarda um tempo antes da próxima iteração
    }
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
    if (mqtt_setup(MQTT_NAME, IP, PORT, MQTT_NAME, MOSQUITTO_PASSWORD)) {
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
        MQTT_RECEIVE_ROOM,  // Tópico a ser assinado
        mqtt_on_request,                        // Callback de confirmação de assinatura
        mqtt_on_incoming_publish,                         // Callback de dados
        mqtt_on_message            // Callback para mensagens recebidas
    );

    connected = true; // Marca que a conexão foi estabelecida
    SIDE_t last_side = ball.side;
    while(true) {
        if (ball.side == side || last_side != ball.side) {
            uint8_t data[64];
            sprintf(data ,"%u,%u,%i,%i,%u", ball.x, ball.y, ball.dx, ball.dy, ball.side);
            bool success = mqtt_comm_publish(MQTT_SEND_ROOM, data, strlen(data) + 1); // Publica a bola atualizada no tópico MQTT
            if (last_side != ball.side) {
                while (!success) {
                    #ifdef DEBUG
                    printf("Falha ao publicar no MQTT, tentando novamente...\n");
                    #endif
                    success = mqtt_comm_publish(MQTT_SEND_ROOM, data, strlen(data) + 1); // Tenta publicar novamente
                    if (!success)
                        vTaskDelay(pdMS_TO_TICKS(TICK_DELAY));
                }
            }
        } else if (mqtt_has_new_data() && ball.side != side) {
            // Se houver novos dados, processa a mensagem recebida
            const char *topic = mqtt_get_last_topic();
            ball = mqtt_get_last_ball(); // Obtém a bola do MQTT
            //unsigned long int timestamp = mqtt_get_last_timestamp();

            #ifdef DEBUG
            //printf("Mensagem recebida no tópico '%s': valor=%.2f, timestamp=%lu\n", topic, value, timestamp);
            #endif
        }
        last_side = ball.side; // Atualiza o último lado da bola
        vTaskDelay(pdMS_TO_TICKS(TICK_DELAY));
    }
}
