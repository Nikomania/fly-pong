#ifndef BALL_H
#define BALL_H

#include <stdint.h>
#include <stdbool.h>

#define TICK_DELAY 62
#define INITIAL_BALL_X 2
#define INITIAL_BALL_Y 4
#define INITIAL_BALL_DX 1
#define INITIAL_BALL_DY -1
#define INITIAL_BALL_SPEED 1
#define INITIAL_BALL_SIDE FLY

#define BAR_SIZE 2

#define INIT_BALL_TICKS 4

#define LED_MATRIX_WIDTH 5
#define LED_MATRIX_HEIGHT 5

typedef enum SIDE_t {
  FLY,
  PONG
} SIDE_t;

extern SIDE_t side;
extern uint8_t bar_pos;
extern uint8_t bar_y;

typedef struct Ball_t {
  int x,y;
  int dx,dy;
  SIDE_t side;
  int speed;
  int points[2];
} Ball_t;

extern Ball_t ball;
extern bool change_side;

void ball_init(SIDE_t side);
void ball_move();

void game_tick();
void game_render();


#endif