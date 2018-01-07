#include "asteroid.h"


struct Asteroid asteroids [MAX_ASTEROIDS] = {};


void
draw_asteroid(struct Asteroid a) {
	draw_poly_offset(a.x, a.y, a.num_vertices, a.vertices);
}

void
add_asteroid(struct Asteroid a) {
	for(int i = 0; i < MAX_ASTEROIDS; i++) {
		if(!asteroids[i].exists) {
			asteroids[i] = a;
			return;
		}
	}
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
destroy_asteroid(struct Asteroid *a) {
	int class = a->class;
	int x = a->x;
	int y = a->y;
	a->exists = false;
	//This is only notequals instead of less than equals because negative asteroids amuse me.
	if(class != 1) {
		add_asteroid(gen_asteroid(x, y, class - 1));
		add_asteroid(gen_asteroid(x, y, class - 1));
	}
	score += 100;
}

void
init_asteroids(void) {
		asteroids[0] = gen_asteroid(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 5);
}

// Updates Asteroids and returns the number remaining
int
tick_asteroids(void) {
	int asteroids_remaining = 0;
	for(int i = 0; i < MAX_ASTEROIDS; i++) {
		//Asteroid Physics
		if(!asteroids[i].exists) {
			continue;
		} else {
			asteroids_remaining++;
		}
		asteroids[i].x += asteroids[i].vx;
		asteroids[i].y += asteroids[i].vy;
		if(bullet_exists && point_in_poly(bx - asteroids[i].x, by - asteroids[i].y, asteroids[i].num_vertices, asteroids[i].vertices)) {
			destroy_asteroid(&asteroids[i]);
			bullet_exists = false;
		}
		//Wrap Asteroids. Buffer zone ensures they do teleport whilst still visible
		float buffer_zone = 2*BASE_RADIUS*1.2*asteroids[i].class;
		if(asteroids[i].x > SCREEN_WIDTH + buffer_zone) {
			asteroids[i].x -= SCREEN_WIDTH + 2*buffer_zone;
		} else if(asteroids[i].x < -buffer_zone) {
			asteroids[i].x += SCREEN_WIDTH + 2*buffer_zone;
		}
		if(asteroids[i].y > SCREEN_HEIGHT + buffer_zone) {
			asteroids[i].y -= SCREEN_HEIGHT + 2*buffer_zone;
		} else if(asteroids[i].y < -buffer_zone) {
			asteroids[i].y += SCREEN_HEIGHT + 2*buffer_zone;
		}
		draw_asteroid(asteroids[i]);

		//Check for Player-Asteroid collision
		if(asteroids[i].exists && poly_in_poly(x, y, 3, ship_poly, asteroids[i].x, asteroids[i].y, asteroids[i].num_vertices, asteroids[i].vertices)) {
			reset_player();
			destroy_asteroid(&asteroids[i]);
			score -= 1000;
			if(score < 0) {
				score = 0;
			}
		}
	}
	return asteroids_remaining;
}
