#include "ship.h"

float x;
float y;
float vx;
float vy;
float rot;
Vec2 ship_poly[3];

// Bullet
float bx;
float by;
float bvx;
float bvy;
bool bullet_exists = false;

void
reset_player(){
	x = SCREEN_WIDTH/2;
	y = 3*SCREEN_HEIGHT/4;
	vx = 0;
	vy = 0;
	rot = -M_PI/2;
}

void
construct_ship_poly(Vec2 verts[static 3]) {
	verts[0] = (Vec2){SHIP_RADIUS*cos(rot), SHIP_RADIUS*sin(rot)};
	verts[1] = (Vec2){SHIP_RADIUS*cos(rot - 3*M_PI/4), SHIP_RADIUS*sin(rot - 3*M_PI/4)};
	verts[2] = (Vec2){SHIP_RADIUS*cos(rot + 3*M_PI/4), SHIP_RADIUS*sin(rot + 3*M_PI/4)};
}

void tick_ships(void) {
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

			// Tick bullet
			if(bullet_exists){
				bx += bvx;
				by += bvy;

				// Wrap
				if(bx > SCREEN_WIDTH || bx < 0 || by > SCREEN_HEIGHT || by < 0) {
					bullet_exists = false;
				}
				SDL_RenderDrawPoint(renderer, bx, by);
			}

			
			if(!bullet_exists && controller_state.shoot) {
				// Create bullet
				bx = x + SHIP_RADIUS*cos(rot);
				by = y + SHIP_RADIUS*sin(rot);
				bvx = vx + 2*BASE_SPEED*cos(rot);
				bvy = vy + 2*BASE_SPEED*sin(rot);
				bullet_exists = true;
				controller_state.shoot = false;
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
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			draw_poly_offset(x, y, 3, ship_poly);

}
