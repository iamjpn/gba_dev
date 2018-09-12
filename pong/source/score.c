#include "score.h"
#include "sprite.h"

Score eScore;
Score pScore;

/* score_reset zeroes score's count and hides all of the counting block */
void score_reset(Score *score) {
  int i;

  for (i = 0; i < score->points; i++) {
    hide_sprite(score->sprite[i]);
  }
  score->points = 0;
}

/* score_inc adds one to score */
void score_inc(Score *score) { score->points++; }

/* score_update shows score's count on the screen */
void score_update(Score *score) {
  int i;
  for (i = 0; i < score->points; i++) {
    show_sprite(score->sprite[i]);
  }
}

/* score_init sets up all of sprite objects used to show score on the screen */
void score_init(Score *score, int x, int y, OBJATTR *sprite_mem) {
  int i;
  score->x = x;
  score->y = y;
  score->points = 0;
  for (i = 0; i < MAX_SCORE; i++) {
    score->sprite[i] = sprite_mem + i;
    obj_set_attr(score->sprite[i], OBJ_Y(y) | ATTR0_COLOR_16 | ATTR0_SQUARE,
                 OBJ_X(x + i * 8) | ATTR1_SIZE_8, ATTR2_PALBANK(0) | 0x09);
    hide_sprite(score->sprite[i]);
  }
}