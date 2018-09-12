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
#include <stdbool.h>
#include "titledata.h"
#include "balldata.h"
#include "paddledata.h"
#include "scoredata.h"
#include "backgrounddata.h"
#include "sprite.h"

#include "gamestate.h"
#include "score.h"
#include "paddle.h"
#include "ball.h"
#include "fixed.h"


#define PLAYER 1
#define COMP 0 
 

#define SCORE_WIDTH 8

/* Initial X position of the player's score */
#define P_SCORE_X (SCREEN_WIDTH / 4) - (SCORE_WIDTH * 3 / 2)

/* Initial X position of the enemy's score */
#define E_SCORE_X (SCREEN_WIDTH / 4 * 3) - (SCORE_WIDTH * 3 / 2)


/* Function Declarations */
void hardware_init(void);


/* hardware_init sets up all of the video memory */
void 
hardware_init() 
{
    /* Prepare the screen */
	background_init();
    REG_DISPCNT = MODE_0 | OBJ_1D_MAP | OBJ_ENABLE | BG0_ENABLE;

    /* Load the sprites into memory */
    memcpy(&tile_mem[4][0], paddleBitmap, paddleBitmapLen);
    memcpy(&tile_mem[4][4], paddleBitmap, paddleBitmapLen);
    memcpy(&tile_mem[4][8], ballBitmap, ballBitmapLen);
    memcpy(&tile_mem[4][9], scoreTiles, scoreTilesLen);

    /* Load the pallete into memory */
    memcpy(SPRITE_PALETTE, paddlePal, paddlePalLen);

    /* Clear the screen to prevent artifacts */
    oam_init(obj_buffer, MAX_SPRITES);
}


void enemy_update(Paddle *paddle, u16 keys) {
    /* Want to calculate where the ball will reach the
     * end of the screen and move to there 
     */
    if (gameState.state == MOVING) {
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


