#ifndef SHIP_H
#define SHIP_H

#include "video.h"
#include "common.h"
#include "geom.h"
#include "control.h"

// Ship data
extern float x;
extern float y;
extern float vx;
extern float vy;
extern float rot;
extern Vec2 ship_poly[3];

// Bullet data
extern float bx;
extern float by;
extern float bvx;
extern float bvy;
extern bool bullet_exists;

void reset_player(void);
void tick_ships(void);

#endif /* SHIP_H */
