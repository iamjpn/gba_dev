#include <gba_console.h>
#include <gba_video.h>
#include <gba_sprites.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "title.h"
#include "ball.h"
#include "paddle.h"
#include "score.h"
#include "background.h"
#include "starbackground.h"

//#include "test.h"
#include "padd.h"
#include "sprite.h"
#include <stdbool.h>

#define BALL_WIDTH 8
#define BALL_HEIGHT 8

#define PLAYER 1
#define COMP 0 

#define PADDLE_WIDTH 8
#define PADDLE_HEIGHT 32 

#define MAX_SPRITES 128
#define SCORE_WIDTH 8

#define P_SCORE_X (SCREEN_WIDTH / 4) - (SCORE_WIDTH * 3 / 2)
#define E_SCORE_X (SCREEN_WIDTH / 4 * 3) - (SCORE_WIDTH * 3 / 2)
#define MAX_SCORE 3

static int timer;
typedef struct Score {
    int x;
    int y;
	int points;
    OBJATTR *sprite[MAX_SCORE];
} Score;

typedef struct Paddle {
    int x;
    int y;
    int width;
    int height;
    OBJATTR *sprite;
	Score *score;
	void (*update_fn)(struct Paddle *, u16 keys);
} Paddle;

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


typedef enum {MOVING, BETWEEN_ROUNDS, WAITING, OVER} State;
typedef struct GameState {
	State state;
    bool moving;
	int pPoints;
	int ePoints;
} GameState;

typedef struct Coord {
    u32 x;
    u32 y;
} Coord;

/* Global Variables */
GameState gameState;
Paddle paddle;
Paddle enemy;
Ball ball;
Score eScore;
Score pScore;

/* Used so that only need to access the VRAM once */
OBJATTR obj_buffer[MAX_SPRITES];

void hide_sprite(OBJATTR *sprite) {
	sprite->attr0 |= (1<<9);
}

void show_sprite(OBJATTR *sprite) {
	sprite->attr0 &= ~(1<<9);
}

void score_reset(Score *score) {
	int i;

	for (i = 0; i < score->points; i++) {
		hide_sprite(score->sprite[i]);
	}
	score->points = 0;
}

void score_inc(Score *score) {
	score->points++;
}

void score_update(Score *score) {
	int i;
	for (i = 0; i < score->points; i++) {
		show_sprite(score->sprite[i]);
	}
}

void score_init(Score *score, int x, int y, OBJATTR *sprite_mem) {
	int i;
	score->x = x;
	score->y = y;
	score->points = 0;
	for(i = 0; i < MAX_SCORE; i++) {
		score->sprite[i] = sprite_mem + i;
		obj_set_attr(score->sprite[i], OBJ_Y(y) | ATTR0_COLOR_16 | ATTR0_SQUARE, 
				OBJ_X(x + i * 8) | ATTR1_SIZE_8,
				ATTR2_PALBANK(0) | 0x09);
		hide_sprite(score->sprite[i]);
	}
}

/* Function Declarations */
void hardware_init(void);
void paddle_init(Paddle *paddle, Score *score, u32 index, u32 x, u32 y, void (*update_fn));
void game_state_init(GameState *gameState);
void ball_init(Ball *paddle);
void paddle_update(Paddle *paddle, u16 keys);
void ball_update(Ball *paddle, u16 keys);
void vsync(void);
void ball_move(Ball *ball);
int collided(Ball *ball, Paddle *paddle, int direction);
bool collided2(Ball *ball, Paddle *paddle, int *xSpeed, int *ySpeed);
int get_sign(int x);
int random_sign();
Coord ball_get_pos(Ball *ball);
Coord paddle_get_pos(Paddle *paddle);
void round_over(Paddle *paddle);
#define FIXED_SIZE 8

void background_init() {
	memcpy(BG_COLORS, starbackgroundPal, starbackgroundPalLen);
	memcpy(&tile_mem[0][0], starbackgroundTiles, starbackgroundTilesLen);
	//memcpy(&tile_mem[0][0],  backgroundTiles, starTilesLen);
	memcpy(&se_mem[30][0], starbackgroundMap, starbackgroundMapLen);
	//REG_MOSAIC = ;
	REG_BG0CNT = CHAR_BASE(0) | SCREEN_BASE(30) | BG_256_COLOR | BG_SIZE_0 | BG_MOSAIC;
}

void background_update() {
	static int ofs;
	REG_BG0HOFS = ofs++;
}

int get_sign(int x) {
    if (x < 0) {
        return -1;
    } else {
        return 1;
    }
}

int from_fix(int x) {
	return x >> FIXED_SIZE;
}

int to_fix(int x) {
	return x << FIXED_SIZE;
}

/* Function Definitions */
void hardware_init() {
    /* Prepare the screen */
	background_init();
    REG_DISPCNT = MODE_0 | OBJ_1D_MAP | OBJ_ENABLE | BG0_ENABLE;

    /* Load the sprites into memory */
    //memcpy(&tile8_mem[4][0], paddaTiles, paddaTilesLen);
    //memcpy(&tile8_mem[4][1], paddTiles, paddTilesLen);
    memcpy(&tile_mem[4][0], paddleBitmap, paddleBitmapLen);
    memcpy(&tile_mem[4][4], paddleBitmap, paddleBitmapLen);
    memcpy(&tile_mem[4][8], ballBitmap, ballBitmapLen);
    memcpy(&tile_mem[4][9], scoreTiles, scoreTilesLen);

    /* Load the pallete into memory */
    memcpy(SPRITE_PALETTE, paddlePal, paddlePalLen);

    /* Clear the screen to prevent artifacts */
    oam_init(obj_buffer, MAX_SPRITES);

}

void game_state_init(GameState *gameState) {
    //gameState->score = 0;
    gameState->pPoints = 0;
    gameState->ePoints = 0;
	gameState->state = WAITING;
    //gameState->moving = false;

}

int paddle_get_score(Paddle *p) {
	return p->score->points;
}

void paddle_init(Paddle *paddle, Score *score, u32 index, u32 x, u32 y, void (*update_fn)) {
    /* The paddles memory is the second in the buffer */
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

void paddle_update(Paddle *paddle, u16 keys) {
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

void enemy_update(Paddle *paddle, u16 keys) {
    /* Want to calculate where the ball will reach the
     * end of the screen and move to there 
     */
    if (gameState.state == MOVING) {
    //if (gameState.moving) {
		Coord ballCoord = ball_get_pos(&ball);
        if(from_fix(ball.x_speed) > 0 && ballCoord.x > 60) {
            int xSpeed = from_fix(ball.x_speed);
            int ySpeed = from_fix(ball.y_speed);

            while (ballCoord.x <= SCREEN_WIDTH - PADDLE_WIDTH) {
                if(ballCoord.y + BALL_HEIGHT  >= SCREEN_HEIGHT) {
                    ySpeed *= -1;
                }
                if(ballCoord.y <= 0) {
                    ySpeed *= -1;
                }
                ballCoord.x += xSpeed;
                ballCoord.y += ySpeed;

            }

            if (BFN_GET(enemy.sprite->attr0, ATTR0_Y) + (PADDLE_HEIGHT / 2) + 5 < ballCoord.y ) {
                enemy.y += 2;
                BFN_SET(enemy.sprite->attr0, enemy.y, ATTR0_Y);
            } else if (BFN_GET(enemy.sprite->attr0, ATTR0_Y) + (PADDLE_HEIGHT / 2) -5 > ballCoord.y) {
                enemy.y -= 2;
                BFN_SET(enemy.sprite->attr0, enemy.y, ATTR0_Y);
            }

            if(enemy.y < 0) {
                enemy.y = 0;
                BFN_SET(enemy.sprite->attr0, enemy.y, ATTR0_Y);
            } else if (enemy.y > SCREEN_HEIGHT - PADDLE_HEIGHT) {
                enemy.y = SCREEN_WIDTH - PADDLE_HEIGHT;
                BFN_SET(enemy.sprite->attr0, enemy.y, ATTR0_Y);

            }
        }
    }
	score_update(paddle->score);

}


int random_sign() {
    if (rand() % 2) {
        return 1;
    }
    return -1;

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
		//game_over();
        //gameState.moving = false;
        //ball_init(ball);
		 round_over(&enemy);
    }

    if(from_fix(ball->x) + BALL_WIDTH > SCREEN_WIDTH -  PADDLE_WIDTH) {
        /* lose state */
        //gameState.moving = false;
        //ball_init(ball);
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

Coord paddle_get_pos(Paddle *paddle) {
    Coord coord;
    coord.x = BFN_GET(paddle->sprite->attr1, ATTR1_X);
    coord.y = BFN_GET(paddle->sprite->attr0, ATTR0_Y);
    return coord;
}

Coord ball_get_pos(Ball *ball) {
    Coord coord;
    //coord.x = BFN_GET(ball->sprite->attr1, ATTR1_X);
    //coord.y = BFN_GET(ball->sprite->attr0, ATTR0_Y);
    coord.x = from_fix(ball->x);
    coord.y = from_fix(ball->y);
    return coord;
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

int collided(Ball *ball, Paddle *paddle, int direction) {
    Coord ballCoord = ball_get_pos(ball);
    Coord paddleCoord = paddle_get_pos(paddle);
    if (ballCoord.x == paddleCoord.x + (PADDLE_WIDTH * direction) &&
            ballCoord.y >= paddleCoord.y &&
            ballCoord.y <= paddleCoord.y + 8) {
        return 4;
    }
    if (ballCoord.x == paddleCoord.x + (PADDLE_WIDTH * direction) &&
            ballCoord.y > paddleCoord.y + 8 &&
            ballCoord.y <= paddleCoord.y + 24) {
        return 3;
    }
    if (ballCoord.x == paddleCoord.x + (PADDLE_WIDTH * direction) &&
            ballCoord.y > paddleCoord.y + 24 &&
            ballCoord.y <= paddleCoord.y + 32) {
        return 4;
    }
    return 0; 
}

void game_reset() {
	ball_init(&ball);
	score_reset(enemy.score);
	score_reset(paddle.score);
}

void game_state_update(GameState *gameState, u16 keys) {
	static int frames;
    if (keys & KEY_A) {
        //gameState->moving = true;
        gameState->state = MOVING;
    } 

	if (gameState->state == BETWEEN_ROUNDS) {
		//frames += 1;
		//if (frames % 21 == 0) {
			//frames = 0;
		timer--;
		if (timer == 0) {
			gameState->state = WAITING;
			ball_init(&ball);
		}
	}

	if (paddle_get_score(&enemy) == MAX_SCORE) {
		game_reset();
	}

	if (paddle_get_score(&paddle) == MAX_SCORE) {
		game_reset();
	}
}

void ball_update(Ball *ball, u16 keys) {
    //if (gameState.moving) {
    if (gameState.state == MOVING) {
        ball_move(ball);
    } 
	if (gameState.state == BETWEEN_ROUNDS) {
		ball_flash(ball);
	}
}

void vsync(void) {
        while(REG_VCOUNT >= 160);   // wait till VDraw
        while(REG_VCOUNT < 160);    // wait till VBlank
}


void play_game(void) {
    hardware_init();
    

	score_init(&pScore, P_SCORE_X, 0, &obj_buffer[4]);
	score_init(&eScore, E_SCORE_X, 0, &obj_buffer[7]);
    paddle_init(&paddle, &pScore, 0, 0, 0, paddle_update);
    paddle_init(&enemy, &eScore, 1, SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2, enemy_update);
    ball_init(&ball);
    game_state_init(&gameState);

    oam_copy(OAM, obj_buffer, 10);

    u16 keyReg;
    setRepeat(1, 1);
    while (1) {
		VBlankIntrWait();
        scanKeys();

        keyReg = keysDownRepeat();
        game_state_update(&gameState, keysDown());
        paddle.update_fn(&paddle, keyReg);
        ball_update(&ball, keysDown());
        enemy.update_fn(&enemy, keyReg);
		background_update();
        oam_copy(OAM, obj_buffer, 10);
    }
}

void show_title_screen(void) {
    u16 keyReg;

	SetMode(3);
	REG_DISPCNT |= BG2_ON;
	memcpy(MODE3_FB, titleBitmap, titleBitmapLen);
	while (1) {
		VBlankIntrWait();
        scanKeys();
        keyReg = keysDownRepeat();
		if (keyReg) {
			break;
		}
	}
}

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	show_title_screen();
	play_game();
}


