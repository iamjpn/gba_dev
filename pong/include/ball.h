#ifndef BALL_H
#define BALL_H

#include <gba_sprites.h>
#include <gba_input.h>
#include <gba_video.h>
#include "score.h"
#include "paddle.h"
#include "coord.h"
#include "sprite.h"

#define BALL_WIDTH 8
#define BALL_HEIGHT 8

/* Ball represents the ball in the game */
typedef struct Ball {
    int x;
    int x_speed;
    int y;
    int y_speed;
    int width;
    int height;
    OBJATTR *sprite;
    bool moving;
	int timer;
} Ball;


extern Ball ball;

void ball_init(Ball *paddle);
void ball_update(Ball *paddle, u16 keys);
void ball_move(Ball *ball);
Coord ball_get_pos(Ball *ball);
void ball_flash(Ball *ball);

#endif 