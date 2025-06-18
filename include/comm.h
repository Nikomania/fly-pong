#include "ball.h"

void init_comm();

void search_comm(SIDE_t side);

void send_state(Ball_t *ball);
void on_receive_state(Ball_t *ball);