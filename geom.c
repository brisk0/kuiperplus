#include "geom.h"

bool
point_in_poly(float x, float y, int num_vert, Vec2 *verts) {
	bool inside = false;
	for(int i = 0, j = num_vert - 1; i < num_vert; j = i++) {
		if((y < verts[i].y && y > verts[j].y) || (y > verts[i].y && y < verts[j].y)) {
			if(x < (verts[j].x - verts[i].x) * (y - verts[i].y) / (verts[j].y - verts[i].y) + verts[i].x) {
				inside = !inside;
			}
		}
	}
	return inside;
}

bool
poly_in_poly(int x1, int y1, int count1, Vec2 *poly1, int x2, int y2, int count2, Vec2 *poly2) {
	for(int i = 0; i < count1; i++) {
		if(point_in_poly(poly1[i].x + x1 - x2, poly1[i].y + y1 - y2, count2, poly2)) {
			return true;
		}
	}
	for(int i = 0; i < count2; i++) {
		if(point_in_poly(poly2[i].x + x2 - x1, poly2[i].y + y2 - y1, count1, poly1)) {
			return true;
		}
	}
	return false;
}

