#ifndef PADDLE_H
#define PADDLE_H

#include <gba_sprites.h>
#include <gba_input.h>
#include <gba_video.h>
#include "score.h"
#include "paddle.h"
#include "coord.h"
#include "sprite.h"

#define PADDLE_WIDTH 8
#define PADDLE_HEIGHT 32

/* Paddle maintains the state and representation of a
   paddle in the game */
typedef struct Paddle {
  int x;
  int y;
  int width;
  int height;
  OBJATTR *sprite;

  /* score used to maintain own points */
  Score *score;
  void (*update_fn)(struct Paddle *, u16 keys);
} Paddle;

extern Paddle paddle;
extern Paddle enemy;

void paddle_update(Paddle *paddle, u16 keys);
void paddle_init(Paddle *paddle, Score *score, u32 index, u32 x, u32 y,
                 void(*update_fn));
Coord paddle_get_pos(Paddle *paddle);
int paddle_get_score(Paddle *p);

#endif