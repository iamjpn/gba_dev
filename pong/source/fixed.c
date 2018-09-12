#include "fixed.h"

/* For fixed point numbers */
#define FIXED_SIZE 8

/* from_fix converts fix->normal */
int from_fix(int x) { return x >> FIXED_SIZE; }

/* from_fix converts normal->fix */
int to_fix(int x) { return x << FIXED_SIZE; }
