#include "asteroid.h"


// TODO Simple unordered deletion (preferably in the list API)
Array(struct Asteroid, asteroids);


void
draw_asteroid(struct Asteroid a) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	draw_poly_offset(a.x, a.y, a.num_vertices, a.vertices);
}

struct Asteroid
gen_asteroid(int x, int y, int class) {
	struct Asteroid a;
	a.x = x;
	a.y = y;
	float v = BASE_SPEED/class*(0.5 + (float)rand()/RAND_MAX);
	float dir = (float)rand()/RAND_MAX * 2*M_PI;
	a.vx = v*cos(dir);
	a.vy = v*sin(dir);
	int vertices = MIN_VERTICES + rand() % (MAX_VERTICES - MIN_VERTICES);
	float lengths[vertices];
	float angles[vertices];
	for(int i = 0; i < vertices; i++) {
		lengths[i] = BASE_RADIUS*class*(0.8 + (float)rand()/RAND_MAX*0.4);
		angles[i] = (i*2+1)*M_PI/(vertices);
		a.vertices[i].x = (int)lengths[i]*cos(angles[i]);
		a.vertices[i].y = (int)lengths[i]*sin(angles[i]);
	}
	a.num_vertices = vertices;
	a.class = class;
	a.exists = true;
	return a;
}

static void
delete_asteroid_idx(unsigned int i) {
	asteroids[i] = asteroids[asteroids_count - 1];
	asteroids_count--;
}

// Normal in-game destruction
static void
destroy_asteroid_idx(unsigned int i) {
	int class = asteroids[i].class;
	int x = asteroids[i].x;
	int y = asteroids[i].y;
	asteroids[i].exists = false;
	delete_asteroid_idx(i);

	//This is only notequals instead of less than equals because negative asteroids amuse me.
	if(class != 1) {
		array_append(asteroids, gen_asteroid(x, y, class - 1));
		array_append(asteroids, gen_asteroid(x, y, class - 1));
	}
	score += 100;
}

void
init_asteroids(void) {
		array_reserve(asteroids, 31);
		array_append(asteroids, gen_asteroid(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 5));

}

// Updates Asteroids and returns the number remaining
int
tick_asteroids(void) {
	//for(int i = 0; i < MAX_ASTEROIDS; i++) { //foreach
	forptr(asteroid, asteroids) {
		//Asteroid Physics
		if(!asteroid->exists) {
			continue;
		}
		asteroid->x += asteroid->vx;
		asteroid->y += asteroid->vy;
		if(bullet_exists && point_in_poly(bx - asteroid->x, by - asteroid->y, asteroid->num_vertices, asteroid->vertices)) {
			destroy_asteroid_idx(asteroid_index);
			bullet_exists = false;
			continue;
		}
		//Wrap Asteroids. Buffer zone ensures they don't teleport whilst still visible
		float buffer_zone = 2*BASE_RADIUS*1.2*asteroid->class;
		if(asteroid->x > SCREEN_WIDTH + buffer_zone) {
			asteroid->x -= SCREEN_WIDTH + 2*buffer_zone;
		} else if(asteroid->x < -buffer_zone) {
			asteroid->x += SCREEN_WIDTH + 2*buffer_zone;
		}
		if(asteroid->y > SCREEN_HEIGHT + buffer_zone) {
			asteroid->y -= SCREEN_HEIGHT + 2*buffer_zone;
		} else if(asteroid->y < -buffer_zone) {
			asteroid->y += SCREEN_HEIGHT + 2*buffer_zone;
		}
		draw_asteroid(*asteroid);

		//Check for Player-Asteroid collision
		if(asteroid->exists && poly_in_poly(x, y, 3, ship_poly, asteroid->x, asteroid->y, asteroid->num_vertices, asteroid->vertices)) {
			reset_player();
			destroy_asteroid_idx(asteroid_index);
			score -= 1000;
			if(score < 0) {
				score = 0;
			}
		}
	}
	return asteroids_count;
}
