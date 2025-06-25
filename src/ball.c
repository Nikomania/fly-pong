#include "ball.h"
#include "button.h"
#include "led_matrix.h"
#include "pico/rand.h"


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

#ifdef MY_SIDE_FLY
int initializing_acc = INIT_BALL_TICKS;
#else
int initializing_acc = 0;
#endif

void ball_init(SIDE_t side)
{
    ball.x = get_rand_32() % LED_MATRIX_WIDTH;
    ball.y = INITIAL_BALL_Y;
    ball.dx = (get_rand_32() % 2 == 0) ? 1 : -1;
    if (ball.x == 0)
        ball.dx = 1; // Garante que a bola não comece na borda esquerda
    else if (ball.x == LED_MATRIX_WIDTH - 1)
        ball.dx = -1; // Garante que a bola não comece na borda direita
    ball.dy = INITIAL_BALL_DY;
    ball.speed = INITIAL_BALL_SPEED;
    ball.side = side;
    initializing_acc = INIT_BALL_TICKS;
}

SIDE_t ball_move() {
    if (initializing_acc) {
        // Se a bola está inicializando, não faz nada
        initializing_acc--;
        return ball.side; // Retorna o lado da bola sem fazer movimento
    }
    // Atualiza a posição da bola
    ball.x += ball.speed * ball.dx;
    ball.y += ball.speed * ball.dy;

    // Verifica colisão com as bordas
    if ((ball.x <= 0 && ball.dx < 0) || (ball.x >= LED_MATRIX_WIDTH-1 && ball.dx > 0) ) {
        ball.dx = -ball.dx; // Inverte a direção horizontal
    }

    if (ball.y >= LED_MATRIX_HEIGHT) {
        ball.side = (ball.side == FLY) ? PONG : FLY; // Troca o lado da bola
        ball.dy = -ball.dy; // Inverte a direção vertical
        ball.y = LED_MATRIX_HEIGHT - 1;
        return ball.side; // Sai da função após a colisão
    }

    // Verifica colisão com a barra do jogador
    if (ball.side == side && ball.y == bar_y+1) {
        for (int i = 0; i < BAR_SIZE; i++) {
            if (ball.x == bar_pos + i) {
                ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
                return ball.side; // Sai da função após a colisão
            }
        }
        if (ball.x + ball.dx == bar_pos || ball.x + ball.dx == bar_pos + BAR_SIZE - 1) {
            ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
            ball.dx = -ball.dx; // Inverte a direção horizontal se a bola está indo para a barra
        }
    }


    return ball.side; // Retorna o lado da bola
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

    // Verifica se a bola saiu da tela
    if (ball.y < 0) {
        ball.points[!side]++;
        // Reinicia a posição da bola
        ball_init(side);
    }
}

void game_render() {
    // Limpa a matriz de LEDs
    clearLEDs();

    static bool ball_on = false;
    // Desenha a bola
    if (ball.side == side) {
        // Se a bola é do lado do jogador, desenha-a
        setLED(ball.x, ball.y, initializing_acc ? YELLOW : WHITE);
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
    render();
}

