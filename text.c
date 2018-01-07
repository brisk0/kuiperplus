#include "text.h"

static SDL_Texture * font_small;
static SDL_Texture * font_medium;
static SDL_Texture * font_large;

//Must be used before text drawing functions or you will segfault probably.
void
load_fonts(SDL_Renderer *renderer) {
	static bool fonts_loaded = false;

	if(fonts_loaded) {
		return;
	}

	bool success = true;
	IMG_Init(IMG_INIT_PNG);
	if(!(font_small = IMG_LoadTexture(renderer, "font_small.png"))) {
		success = false;
	}
	if(!(font_medium = IMG_LoadTexture(renderer, "font_medium.png"))) {
		success = false;
	}
		IMG_Quit();
		fonts_loaded = success;

		if(!success) {
			fprintf(stderr, "Failed to load fonts; Aborted");
			exit(EXIT_FAILURE);
		}
}

// Writes a string to the screen using a position anchor and alignment descriptions.
// Currently assumes fonts are square, font size is pixel size.
void
draw_string(SDL_Renderer * renderer, char * string, int x, int y, enum FontSize font_size, enum Align align, enum AlignVert align_vert) {

	load_fonts(renderer);

	SDL_Texture *font;
	switch(font_size) {
		case small:
			font = font_small;
			break;
		case medium:
			font = font_medium;
			break;
		case large:
			font = font_large;
			break;
	}
	if(align != left) {
		int num_chars = 0;
		while(string[num_chars]) {
			num_chars++;
		}
		if(align == right) {
			x -= num_chars*font_size;
		} else if(align == center) {
			x -= num_chars*font_size/2;
		}
	}
	if(align_vert == top) {
		y -= font_size;
	} else if(align_vert == middle) {
		y -= font_size/2;
	}
	int i = 0;

	while(string[i]) {
		SDL_Rect src = (SDL_Rect){(string[i] - 32)*font_size, 0, font_size, font_size};
		SDL_Rect dest = (SDL_Rect){x + i*font_size, y, font_size, font_size};
		SDL_RenderCopy(renderer, font, &src, &dest);
		i++;
	}
}
