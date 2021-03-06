#ifndef COMMON_H
#define COMMON_H
// Global pointers and definitions that need to be broadly accessible.
#include <SDL2/SDL_render.h>
#include "geom.h"
#include <stdbool.h>

int score;

//TODO most of these can go in a specific place
#define MAX_ASTEROIDS 10
#define MIN_VERTICES 5
#define MAX_VERTICES 12
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define SHIP_RADIUS 8
#define SCREEN_BORDER SHIP_RADIUS
#define BASE_SPEED 0.53*2
#define BASE_RADIUS 8
#define MAX_VEL BASE_SPEED
#define ACCEL MAX_VEL/60
#define ROT_SPEED 2*M_PI/60



#endif
