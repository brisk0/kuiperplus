#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tgmath.h>
#include <unistd.h>

#include "common.h"
#include "control.h"
#include "text.h"
#include "geom.h"
#include "asteroid.h"
#include "video.h"
#include "ship.h"

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
								if(!event.key.repeat) {
									controller_state.shoot = true;
									if(!bullet_exists) {
										Mix_PlayChannel(-1, sound_shoot, 0);
									}
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
			
			tick_ships();

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
