#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL_render.h>
#include "common.h"
#include "geom.h"

SDL_Renderer* renderer;

void
draw_poly_offset(int x, int y, int num_vertices, Vec2 *vertices);


#endif
