#include <gba_console.h>
#include <stdbool.h>
#include <gba_input.h>
#include "gamestate.h"
#include "ball.h"
#include "score.h"

GameState gameState;
int timer;


/* game_state_init sets the game back to waiting to begin */
void 
game_state_init(GameState *gameState) 
{
    gameState->pPoints = 0;
    gameState->ePoints = 0;
	gameState->state = WAITING;
}

void game_state_reset() {
	ball_init(&ball);
	score_reset(enemy.score);
	score_reset(paddle.score);
}

void game_state_update(GameState *gameState, u16 keys) {
	static int frames;
    if (keys & KEY_A) {
        gameState->state = MOVING;
    } 

	if (gameState->state == BETWEEN_ROUNDS) {
		timer--;
		if (timer == 0) {
			gameState->state = WAITING;
			ball_init(&ball);
		}
	}

	if (paddle_get_score(&enemy) == MAX_SCORE) {
		game_state_reset();
	}

	if (paddle_get_score(&paddle) == MAX_SCORE) {
		game_state_reset();
	}
}