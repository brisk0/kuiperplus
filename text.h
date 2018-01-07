#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>

enum Align {
	left,
	right,
	center
};

enum AlignVert {
	top,
	bottom,
	middle
};

enum FontSize {
	small = 8,
	medium = 16,
	large = 32
};

void draw_string(SDL_Renderer * renderer, char * string, int x, int y, enum FontSize font_size, enum Align align, enum AlignVert align_vert);

#endif
