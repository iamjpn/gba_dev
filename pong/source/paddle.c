#include "paddle.h"

Paddle paddle;
Paddle enemy;

/* paddle_get_score returns the points a paddle has scored */
int 
paddle_get_score(Paddle *p) 
{
	return p->score->points;
}


/* paddle_init sets up paddle. 
   update_fn will be called every frame to update the paddle
   index is the offset of the paddle in object memory */
void 
paddle_init(Paddle *paddle, Score *score, u32 index, u32 x, u32 y, void (*update_fn)) 
{
    paddle->sprite = &obj_buffer[index];
    paddle->x = x;
    paddle->y = y;
	paddle->score = score;
    paddle->width = PADDLE_WIDTH;
    paddle->height = PADDLE_HEIGHT;
	paddle->update_fn = update_fn;

    /* 02 cause double size pallete */
    obj_set_attr(paddle->sprite, OBJ_Y(y) | ATTR0_COLOR_16 | ATTR0_TALL, 
            OBJ_X(x) | ATTR1_SIZE_16,
            ATTR2_PALBANK(0) | index);
}

/* paddle_update moves a paddle in response to pressed buttons */
void 
paddle_update(Paddle *paddle, u16 keys) 
{
    if (keys & KEY_DOWN) {
        paddle->y += 2;
        if (paddle->y + PADDLE_HEIGHT > SCREEN_HEIGHT) {
            paddle->y = SCREEN_HEIGHT - PADDLE_HEIGHT;
        }
        BFN_SET(paddle->sprite->attr0, paddle->y, ATTR0_Y);
    }

    if (keys & KEY_UP) {
        paddle->y -= 2;
        if (paddle->y > SCREEN_HEIGHT || paddle->y < 0) {
            paddle->y = 0;
        }
        BFN_SET(paddle->sprite->attr0, paddle->y, ATTR0_Y);
    }
	score_update(paddle->score);
}

Coord paddle_get_pos(Paddle *paddle) {
    Coord coord;
    coord.x = BFN_GET(paddle->sprite->attr1, ATTR1_X);
    coord.y = BFN_GET(paddle->sprite->attr0, ATTR0_Y);
    return coord;
}

