#include "ball.h"
#include "button.h"
#include "led_matrix.h"
#include "pico/rand.h"
#include "oled.h"


SIDE_t side;
uint8_t bar_pos = 0;
uint8_t bar_y = 0;
Ball_t ball = {
    .x = INITIAL_BALL_X,
    .y = INITIAL_BALL_Y,
    .dx = INITIAL_BALL_DX,
    .dy = INITIAL_BALL_DY,
    .side = INITIAL_BALL_SIDE,
    .speed = INITIAL_BALL_SPEED,
    .points = {0, 0}
};

bool change_side = false;

#ifdef MY_SIDE_FLY
static volatile int initializing_acc = INIT_BALL_TICKS;
#else
static volatile int initializing_acc = 0;
#endif

void ball_init(SIDE_t side)
{
    ball.x = get_rand_32() % LED_MATRIX_WIDTH;
    ball.y = INITIAL_BALL_Y;
    ball.dx = (get_rand_32() % 2 == 0) ? 1 : -1;
    if (ball.x == 0)
        ball.dx = 1; // Garante que a bola não saia da tela
    else if (ball.x == LED_MATRIX_WIDTH - 1)
        ball.dx = -1; // o mesmo
    ball.dy = INITIAL_BALL_DY;
    ball.speed = INITIAL_BALL_SPEED;
    ball.side = side;
    initializing_acc = INIT_BALL_TICKS;
}

void ball_move() {
    // Verifica se a bola saiu da tela
    if (ball.y < 0) {
        printf("Bola saiu da tela (ponto para pong)!\n");
        ball.points[!side]++;
        // Reinicia a posição da bola
        ball_init(side);
    }

    if (initializing_acc > 0) {
        // Se a bola está inicializando, não faz nada
        initializing_acc--;
        return;
    }
    // Atualiza a posição da bola
    ball.x += ball.speed * ball.dx;
    ball.y += ball.speed * ball.dy;

    // Verifica colisão com as bordas
    if ((ball.x <= 0 && ball.dx < 0) || (ball.x >= LED_MATRIX_WIDTH-1 && ball.dx > 0) ) {
        ball.dx = -ball.dx; // Inverte a direção horizontal
    }

    if (ball.y >= LED_MATRIX_HEIGHT && ball.dy > 0) {
        change_side = true;
        ball.side = (ball.side == FLY) ? PONG : FLY; // Troca o lado da bola
        ball.dy = -1; // Faz a bola cair no lado correto
        ball.y = LED_MATRIX_HEIGHT - 1;
        return; // Sai da função após a colisão
    }

    // Verifica colisão com a barra do jogador
    if (ball.y == bar_y+1) {
        for (int i = 0; i < BAR_SIZE; i++) {
            if (ball.x == bar_pos + i) {
                ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
                return; // Sai da função após a colisão
            }
        }
        if (ball.x + ball.dx == bar_pos || ball.x + ball.dx == bar_pos + BAR_SIZE - 1) {
            ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
            ball.dx = -ball.dx; // Inverte a direção horizontal se a bola está indo para a barra
        }
    }
}

void game_tick() {
    if (read_button(BTN_A) == BTN_PRESSED && read_button(BTN_B) == BTN_PRESSED) {
        if (bar_y == 2) {
            bar_y = 0;
        } else {
            bar_y += 1;
        }
    } else if (read_button(BTN_A) == BTN_PRESSED) {
        // Move a barra do jogador para a esquerda
        if (bar_pos > 0) {
            bar_pos--;
        }
    } else if (read_button(BTN_B) == BTN_PRESSED) {
        // Move a barra do jogador para a direita
        if (bar_pos < LED_MATRIX_WIDTH - BAR_SIZE) {
            bar_pos++;
        }
    }

    if (ball.side != side) {
        // Se a bola não é do lado do jogador, não faz nada
        return;
    }

    static bool updating = true;
    if (updating)
        ball_move();
    updating = !updating; 
}

void game_render() {
    // Limpa a matriz de LEDs
    clearLEDs();

    static bool ball_on = false;
    // Desenha a bola
    if (ball.side == side) {
        // Se a bola é do lado do jogador, desenha-a
        setLED(ball.x, ball.y, initializing_acc > 0 ? YELLOW : WHITE);
    } else {
        // Se a bola é do lado do oponente, desenha-a em branco
        if (ball_on) {
            setLED(ball.x, LED_MATRIX_HEIGHT-1, WHITE);
        } else {
            setLED(ball.x, LED_MATRIX_HEIGHT-1, BLACK);
        }
        ball_on = !ball_on;
    }

    // Desenha a barra do jogador
    for (int i = 0; i < BAR_SIZE; i++) {
        setLED(bar_pos + i, bar_y, BLUE);
    }

    // Renderiza a matriz de LEDs
    renderLedMatrix();

    clear_OLed();

    char fly_points_str[17];
    sprintf(fly_points_str, "%16d", ball.points[FLY]);
    char pong_points_str[17];
    sprintf(pong_points_str, "%16d", ball.points[PONG]);

    char* texts[] = {
        "     POINTS     ",
        "                ",
        "      FLY       ",
        fly_points_str,
        "                ",
        "      PONG      ",
        pong_points_str
    };
    print_lines_OLed(texts, 7, 0, 0);

    render_OLed();
}

