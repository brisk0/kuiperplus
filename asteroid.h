#ifndef ASTEROID_H
#define ASTEROID_H

#include "geom.h"
#include "video.h"
#include "array.h"
#include "ship.h"
#include <stdbool.h>
#include <tgmath.h>

struct Asteroid {
	bool exists;
	float x;
	float y;
	float vx;
	float vy;
	int num_vertices;
	Vec2 vertices[MAX_VERTICES];
	int class;
};

void init_asteroids(void);

int tick_asteroids(void);

#endif
