#include "common.h"
#include "control.h"
#include "text.h"
#include "geom.h"
#include "asteroid.h"
#include "video.h"
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_mixer.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tgmath.h>
#include <unistd.h>


// Ship
float x;
float y;
float vx;
float vy;
float rot;
Vec2 ship_poly[3];

void
reset_player(){
	x = SCREEN_WIDTH/2;
	y = 3*SCREEN_HEIGHT/4;
	vx = 0;
	vy = 0;
	rot = -M_PI/2;
}

// Bullet
float bx;
float by;
float bvx;
float bvy;
bool bullet_exists = false;

int score;
char score_string[1000];

SDL_Window* window;

//Sounds
Mix_Music *music;
Mix_Chunk *sound_shoot;
Mix_Chunk *sound_engine;
int channel_sound_engine;

void
init() {
	//Check SDL Version
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	if (compiled.major != linked.major) {
		fprintf(stderr, "SDL version mismatch! Found version %d, require version %d", linked.major, compiled.major);
		exit(EXIT_FAILURE);
	}


	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO)) {
		fprintf(stderr, "Initialisation Error: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	} else {
		atexit(SDL_Quit);
	}

	//Hints
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	//Enable controllers
	SDL_GameControllerEventState(SDL_ENABLE);
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

	//Sound and Music
	Mix_Init(0);
	atexit(Mix_Quit);
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096)) {
		printf("Error opening audio: %s\n", Mix_GetError());
	} else {
		atexit(Mix_CloseAudio);
	}

	music = Mix_LoadMUS("bgm.wav");
	Mix_PlayMusic(music, -1);
	sound_shoot = Mix_LoadWAV("shoot.wav");
	sound_engine = Mix_LoadWAV("engine.wav");
	channel_sound_engine = Mix_PlayChannel(-1, sound_engine, -1);
	Mix_Pause(channel_sound_engine);
}

void
construct_ship_poly(Vec2 verts[static 3]) {
	verts[0] = (Vec2){SHIP_RADIUS*cos(rot), SHIP_RADIUS*sin(rot)};
	verts[1] = (Vec2){SHIP_RADIUS*cos(rot - 3*M_PI/4), SHIP_RADIUS*sin(rot - 3*M_PI/4)};
	verts[2] = (Vec2){SHIP_RADIUS*cos(rot + 3*M_PI/4), SHIP_RADIUS*sin(rot + 3*M_PI/4)};
}

void
wait_for_key(SDL_Keycode sym) {
	bool wait = true;
	SDL_Event event;
	while(wait) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == sym) {
					wait = false;
				}
			} else if(event.type == SDL_QUIT) {
				exit(EXIT_SUCCESS);
			}
		}
		SDL_Delay(16);
	}
}

int
main(int argc, char **argv) {
	init();
	srand(getpid());

	if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
		fprintf(stderr, "Window creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetWindowTitle(window, "Kuiper");

	//Splash Screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	draw_string(renderer, "Brandon Blake's", SCREEN_WIDTH/2, 3, small, center, bottom);
	draw_string(renderer, "KUIPER!", SCREEN_WIDTH/2, 2*SCREEN_HEIGHT/5, medium, center, middle);
	draw_string(renderer, "Press [space] to Start!", SCREEN_WIDTH/2, 3*SCREEN_HEIGHT/5, small, center, middle);
	draw_string(renderer, "Music by Kaelaholme, CC0", SCREEN_WIDTH/2, SCREEN_HEIGHT - 3, small, center, top);
	SDL_Delay(100);
	SDL_RenderPresent(renderer);
	wait_for_key(SDLK_SPACE);

	while(1) {
		//set up for game start
		bool game_on = true;
		init_asteroids();
		reset_player();
		score = 0;

		//game loop
		int t = SDL_GetTicks();
		int dt = 0;
		SDL_Event event;
		while(game_on) {
			//Begin Frame
			dt = SDL_GetTicks() - t;
			t += dt;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			//Draw Score
			sprintf(score_string, "%d", score);
			draw_string(renderer, score_string, 0, 0, small, left, bottom);

			//EVENT LOOP
			while(SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_KEYDOWN:
						switch(event.key.keysym.sym) {
							case SDLK_RIGHT:
								controller_state.right = true;
								break;
							case SDLK_LEFT:
								controller_state.left = true;
								break;
							case SDLK_UP:
								controller_state.up = true;
								Mix_Resume(channel_sound_engine);
								break;
							case SDLK_DOWN:
								controller_state.down = true;
								break;
							case SDLK_SPACE:
								controller_state.shoot = true;
								if(!bullet_exists && !event.key.repeat) {
									bx = x + SHIP_RADIUS*cos(rot);
									by = y + SHIP_RADIUS*sin(rot);
									bvx = vx + 2*BASE_SPEED*cos(rot);
									bvy = vy + 2*BASE_SPEED*sin(rot);
									bullet_exists = true;
									Mix_PlayChannel(-1, sound_shoot, 0);
								}
								break;

						}
						break;
					case SDL_KEYUP:
						switch(event.key.keysym.sym) {
							case SDLK_RIGHT:
								controller_state.right = false;
								break;
							case SDLK_LEFT:
								controller_state.left = false;
								break;
							case SDLK_UP:
								controller_state.up = false;
								Mix_Pause(channel_sound_engine);
								break;
							case SDLK_DOWN:
								controller_state.down = false;
								break;
							case SDLK_SPACE:
								controller_state.shoot = false;
								break;

						}
						break;
						//System Events
					case SDL_QUIT:
						exit(EXIT_SUCCESS);
				}
			}
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			//player physics
			x += vx;
			y += vy;
			if(controller_state.up) {
				vx += ACCEL*cos(rot);
				vy += ACCEL*sin(rot);
				float magvel = sqrt(vx*vx + vy*vy);
				if (magvel > MAX_VEL) {
					vx = vx/magvel*MAX_VEL;
					vy = vy/magvel*MAX_VEL;
				}
			}
			if(controller_state.right && !controller_state.left) {
				rot += ROT_SPEED;
			} else if (controller_state.left && !controller_state.right) {
				rot -= ROT_SPEED;
			}
			//player wrap
			if(x > SCREEN_WIDTH + SCREEN_BORDER) {
				x -= SCREEN_WIDTH + 2*SCREEN_BORDER;
			} else if(x < -SCREEN_BORDER) {
				x += SCREEN_WIDTH + 2*SCREEN_BORDER;
			}
			if(y > SCREEN_HEIGHT + SCREEN_BORDER) {
				y -= SCREEN_HEIGHT + 2*SCREEN_BORDER;
			} else if(y < -SCREEN_BORDER) {
				y += SCREEN_HEIGHT + 2*SCREEN_BORDER;
			}
			construct_ship_poly(ship_poly);
			draw_poly_offset(x, y, 3, ship_poly);

			//bullet
			if(bullet_exists){
				bx += bvx;
				by += bvy;

				if(bx > SCREEN_WIDTH || bx < 0 || by > SCREEN_HEIGHT || by < 0) {
					bullet_exists = false;
				}
				SDL_RenderDrawPoint(renderer, bx, by);
			}

			//Asteroids
			if(!tick_asteroids()) {
				game_on = false;
			}

			//End Frame
			SDL_RenderPresent(renderer);
			dt = SDL_GetTicks() - t;
			// Lock to just under 60fps
			if(dt < 16) {
				SDL_Delay(16 - dt);
			}
		}
		//Game Ended
		draw_string(renderer, "Your Score:", SCREEN_WIDTH/2, SCREEN_HEIGHT/3, small, center, middle);
		draw_string(renderer, score_string, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, medium, center, middle);
		draw_string(renderer, "Press 'R' to Restart", SCREEN_WIDTH/2, 2*SCREEN_HEIGHT/3, small, center, middle);
		SDL_RenderPresent(renderer);
		wait_for_key(SDLK_r);
	}
	return 0;
}
