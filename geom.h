#ifndef GEOM_H
#define GEOM_H

#include <stdbool.h>

typedef struct {
		float x;
		float y;
} Vec2;

// Migrate polygons to this format
typedef struct {
       unsigned int len;
       Vec2 points[];
} Poly;

bool point_in_poly(float x, float y, int num_vert, Vec2 *verts);

bool poly_in_poly(int x1, int y1, int count1, Vec2 *poly1, int x2, int y2, int count2, Vec2 *poly2);

#endif
