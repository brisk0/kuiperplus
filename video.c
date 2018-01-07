#include "video.h"

void
draw_poly_offset(int x, int y, int num_vertices, Vec2 *vertices) {
	for(int i = 0; i < num_vertices - 1; i++) {
		SDL_RenderDrawLine(renderer, x + vertices[i].x, y + vertices[i].y, x + vertices[i+1].x, y + vertices[i+1].y);
	}
	SDL_RenderDrawLine(renderer, x + vertices[num_vertices - 1].x, y + vertices[num_vertices - 1].y, x + vertices[0].x, y + vertices[0].y);
}
