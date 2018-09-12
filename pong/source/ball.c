#include <stdlib.h>
#include "ball.h"
#include "fixed.h"
#include "gamestate.h"
#include "paddle.h"

Ball ball;

int random_sign() {
    if (rand() % 2) {
        return 1;
    }
    return -1;
}

/* get_sign returns the sign of x */
int 
get_sign(int x) 
{
    if (x < 0) {
        return -1;
    } else {
        return 1;
    }
}

Coord ball_get_pos(Ball *ball) {
    Coord coord;
    coord.x = from_fix(ball->x);
    coord.y = from_fix(ball->y);
    return coord;
}


void ball_init(Ball *ball) {
    ball->sprite = &obj_buffer[2];
    ball->x = to_fix(SCREEN_WIDTH / 2); 
    ball->x_speed = to_fix(4) * random_sign(); 
	ball->timer = 0;

    ball->y =  to_fix(SCREEN_HEIGHT / 2); 
    ball->y_speed = to_fix(0)  * random_sign();

    ball->width = BALL_WIDTH;
    ball->height = BALL_HEIGHT;

    ball->moving = false;

    obj_set_attr(ball->sprite, OBJ_Y(from_fix(ball->y)) | ATTR0_COLOR_16 | ATTR0_SQUARE, 
            OBJ_X(from_fix(ball->x)) | ATTR1_SIZE_8,
            ATTR2_PALBANK(0) | 0x08);
}

void ball_flash(Ball *ball) {
	int pal;

	if (ball->timer == 0) {
		pal = BFN_GET(ball->sprite->attr2, ATTR2_PALBANK);
		pal = !pal;
		BFN_SET(ball->sprite->attr2, pal, ATTR2_PALBANK);
		ball->timer = 5;
	}
	ball->timer--;
}

void ball_update(Ball *ball, u16 keys) {
    if (gameState.state == MOVING) {
        ball_move(ball);
    } 
	if (gameState.state == BETWEEN_ROUNDS) {
		ball_flash(ball);
	}
}






bool collided2(Ball *ball, Paddle *paddle, int *xSpeed, int *ySpeed) {
    /* Get the top left corner */
    /* y axis increases from top to bottom */
    Coord ballCoord = ball_get_pos(ball);
    Coord paddleCoord = paddle_get_pos(paddle);


    /* First check the x coords are fine */
    /* If the ball has a negative speed it is moving left */
    int addWidth = 0;
    int addBallWidth = 0;
    if(from_fix(ball->x_speed) < 0) {
        addWidth = 1;
        addBallWidth = 0;
    } else {
       addWidth = 0; 
       addBallWidth = 1;
    }

    if(ballCoord.x + (BALL_WIDTH * addBallWidth) == paddleCoord.x + (PADDLE_WIDTH * addWidth)) {
        /* Check the bottom of the ball against the top of the paddle */
        if (ballCoord.y + BALL_HEIGHT >= paddleCoord.y && 
                ballCoord.y + BALL_HEIGHT <= paddleCoord.y + (PADDLE_HEIGHT / 3)) {
            *xSpeed = -1 * get_sign(*xSpeed) * to_fix(4);
            *ySpeed = to_fix(2) * get_sign(*ySpeed);
            return true;
        }

        if (ballCoord.y  >= paddleCoord.y + ((PADDLE_HEIGHT / 3) *2) && 
                ballCoord.y <= paddleCoord.y + PADDLE_HEIGHT) {
            *xSpeed = -1 * get_sign(*xSpeed) * to_fix(4);
            *ySpeed = to_fix(2) * get_sign(*ySpeed);
            return true;
        }

        if (ballCoord.y + BALL_HEIGHT >= paddleCoord.y && 
                ballCoord.y <= paddleCoord.y + PADDLE_HEIGHT) {
            *xSpeed = -1 * get_sign(*xSpeed) * to_fix(4);
            *ySpeed = to_fix(1) * get_sign(*ySpeed);
            return true;
        }

    }
    return false;
}

void round_over(Paddle *p) {
	//ball_flash(&ball);
	//gameState.moving = false;
	score_inc(p->score);
	gameState.state = BETWEEN_ROUNDS;
	timer = 100;
}



void ball_move(Ball *ball) {

    if (collided2(ball, &paddle, &(ball->x_speed), &(ball->y_speed))) {
        ;
    }

    if (collided2(ball, &enemy, &(ball->x_speed), &(ball->y_speed))) {
        ;
    }


    /* Wall Collisions */
    if(from_fix(ball->x) + BALL_WIDTH > SCREEN_WIDTH - PADDLE_WIDTH) {
        ball->x_speed *= -1;
    }

    if(from_fix(ball->x) < PADDLE_WIDTH) {
		 round_over(&enemy);
    }

    if(from_fix(ball->x) + BALL_WIDTH > SCREEN_WIDTH -  PADDLE_WIDTH) {
        /* lose state */
		round_over(&paddle);
    }

    if(from_fix(ball->y) + BALL_HEIGHT  >= SCREEN_HEIGHT) {
        ball->y_speed *= -1;
    }
    if(from_fix(ball->y) <= 0) {
        ball->y_speed *= -1;
    }

    /* Update ball with speed */
    ball->x += ball->x_speed;
    ball->y += ball->y_speed;

    obj_set_pos(ball->sprite, from_fix(ball->x), from_fix(ball->y));
}