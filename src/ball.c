#include "ball.h"
#include "button.h"
#include "led_matrix.h"

SIDE_t side;
uint8_t bar_pos;
Ball_t ball;

void ball_init(SIDE_t side) {
    ball.x = INITIAL_BALL_X;
    ball.y = INITIAL_BALL_Y;
    ball.dx = INITIAL_BALL_DX;
    ball.dy = INITIAL_BALL_DY;
    ball.side = side;
}

SIDE_t ball_move() {
    // Atualiza a posição da bola
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Verifica colisão com as bordas
    if (ball.x < 0 || ball.x >= LED_MATRIX_WIDTH-1) {
        ball.dx = -ball.dx; // Inverte a direção horizontal
    }

    if (ball.y >= LED_MATRIX_HEIGHT) {
        ball.side = (ball.side == FLY) ? PONG : FLY; // Troca o lado da bola
        return ball.side; // Sai da função após a colisão
    }

    // Verifica colisão com a barra do jogador
    if (ball.side == side && ball.y == 1) {
        for (int i = 0; i < BAR_SIZE; i++) {
            if (ball.x == bar_pos + i) {
                ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
                return ball.side; // Sai da função após a colisão
            }
        }
        if (ball.x + ball.dx == bar_pos || ball.x + ball.dx == bar_pos + BAR_SIZE - 1) {
            ball.dy = -ball.dy; // Inverte a direção vertical ao colidir com a barra
            ball.dx = -ball.dx; // Inverte a direção horizontal se a bola está indo para a barra
            return ball.side; // Sai da função após a colisão
        }
    }


    return ball.side; // Retorna o lado da bola
}

void game_tick() {
    if (read_button(BTN_A) == BTN_PRESSED) {
        // Move a barra do jogador para a esquerda
        if (bar_pos > 0) {
            bar_pos--;
        }
    }
    if (read_button(BTN_B) == BTN_PRESSED) {
        // Move a barra do jogador para a direita
        if (bar_pos < LED_MATRIX_WIDTH - BAR_SIZE) {
            bar_pos++;
        }
    }

    if (ball.side != side) {
        // Se a bola não é do lado do jogador, não faz nada
        return;
    }
    // Move a bola
    ball_move();


    // Verifica se a bola saiu da tela
    if (ball.y < 0) {
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
        setLED(ball.x, ball.y, WHITE);
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
        setLED(bar_pos + i, 0, BLUE);
    }

    // Renderiza a matriz de LEDs
    render();
}

