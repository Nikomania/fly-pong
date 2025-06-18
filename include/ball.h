#ifndef BALL_H
#define BALL_H

#include <stdint.h>

#define TICK_DELAY 500
#define INITIAL_BALL_X 2
#define INITIAL_BALL_Y 4
#define INITIAL_BALL_DX 1
#define INITIAL_BALL_DY -1
#define INITIAL_BALL_SPEED 1

#define BAR_SIZE 2

#define LED_MATRIX_WIDTH 5
#define LED_MATRIX_HEIGHT 5

typedef enum SIDE_t {
  FLY,
  PONG
} SIDE_t;

extern SIDE_t side;
extern uint8_t bar_pos;

typedef struct Ball_t {
  int x,y;
  int8_t dx,dy;
  SIDE_t side;
  int speed;
} Ball_t;

extern Ball_t ball;

void ball_init(SIDE_t side);
SIDE_t ball_move();

void game_tick();
void game_render();


#endif