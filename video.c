#include "video.h"

// Bresenham's line algorithm, workaround of SDL_RenderDrawLine bug 2019-08-20
int DrawLine(SDL_Renderer *renderer, int x0, int y0, int x1, int y1) {
	float dx  = abs(x1 - x0);
	float dy  = -abs(y1 - y0);
	int sx    = x0 < x1 ? 1 : -1;
	int sy    = y0 < y1 ? 1 : -1;
	float err = dx + dy;
	while(true) {
		if(x0 == x1 && y0 == y1) break;
		float e2 = 2 * err;
		if(e2 >= dy) {
			err += dy; /* e_xy+e_x > 0 */
			x0 += sx;
		}
		if(e2 <= dx){ /* e_xy+e_y < 0 */
			err += dx;
			y0 += sy;
		}
		SDL_RenderDrawPoint(renderer, x0, y0);
	}
	return 0;
}

void
draw_poly_offset(int x, int y, int num_vertices, Vec2 *vertices) {
	for(int i = 0; i < num_vertices; i++) {
		if (DrawLine(renderer, x + vertices[i].x, y + vertices[i].y, x + vertices[(i+1)%num_vertices].x, y + vertices[(i+1)%num_vertices].y)) {
			printf("RenderDrawLine failed: %s\n", SDL_GetError());
		}
	}
}
