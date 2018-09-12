#ifndef GAMESTATE_H
#define GAMESTATE_H



/* GameState maintains a global state for the game */
typedef enum {MOVING, BETWEEN_ROUNDS, WAITING, OVER} State;
typedef struct GameState {
	State state;
  bool moving;
	int pPoints;
	int ePoints;
} GameState;

extern GameState gameState;
extern int timer;

void game_state_init(GameState *gameState);
void game_state_update(GameState *gameState, u16 keys);

#endif