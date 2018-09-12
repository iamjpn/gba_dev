#ifndef SCORE_H
#define SCORE_H
#include <gba_sprites.h>
/* Score maintains the amount of points a paddle has won
   and represents as a sprite */

/* Amount of points needed to win a round */
#define MAX_SCORE 3
typedef struct Score {
  int x;
  int y;
  int points;
  OBJATTR *sprite[MAX_SCORE];
} Score;

extern Score eScore;
extern Score pScore;

void score_reset(Score *score);
void score_inc(Score *score);
void score_update(Score *score);
void score_init(Score *score, int x, int y, OBJATTR *sprite_mem);
#endif