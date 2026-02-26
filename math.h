#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

#define VECTOR3_ZERO (Vector3){ 0.0f, 0.0f, 0.0f }
#define VECTOR3_UP (Vector3){ 0.0f, 1.0f, 0.0f }
#define VECTOR3_DOWN (Vector3){ 0.0f, -1.0f, 0.0f }
#define VECTOR3_FORWARD (Vector3){ 0.0f, 0.0f, 1.0f }
#define VECTOR3_BACKWARD (Vector3){ 0.0f, 0.0f, -1.0f }
#define VECTOR3_RIGHT (Vector3){ 1.0f, 0.0f, 0.0f }
#define VECTOR3_LEFT (Vector3){ -1.0f, 0.0f, 0.0f }
#define VECTOR3_INFINITY (Vector3){ INFINITY, INFINITY, INFINITY }

#ifndef EPSILON
	#define EPSILON 0.0001f
#endif

/* Returns the minimum of 3 numeric values. */
#define MIN3(a,b,c) (((a) < (b)) \
	? ((a) < (c)) ? (a) : (c) \
	: ((b) < (c)) ? (b) : (c))
	
/* Returns the maximum of 3 numeric values. */
#define MAX3(a,b,c) (((a) > (b)) \
	? ((a) > (c)) ? (a) : (c) \
	: ((b) > (c)) ? (b) : (c))

/* Branchless absolute value for floats. Clears the sign bit */
f32 fn math_f32_abs(f32 value);

/* A short, fast, branchless version of floor, rounding down to the nearest integer. */
f32 fn math_f32_floor(f32 value);

/* A short, fast, branchless version of ceiling, rounding up to the nearest integer. */
f32 fn math_f32_ceiling(f32 value);

Vector3 fn math_triplane_line_intersection(
	Vector3 tri_point_1,
	Vector3 tri_point_2,
	Vector3 tri_point_3,

	Vector3 line_0, float a, float b, float c
);

/**
 * Returns the pointer where a line defined by (x_0 + at, y_0 + bt, z_0 + ct) intersects with a given triangle
 * 
 * Returns infinity in the event that it doesn't.
 */
Vector3 fn math_line_triangle_intersection(
	Vector3 tri_point_1,
	Vector3 tri_point_2,
	Vector3 tri_point_3,

	/* line = (x_0 + at, y_0 + bt, z_0 + ct) */
	Vector3 line_0, float a, float b, float c
);

/* Extracts the transform matrix from a Transform struct. */
Matrix fn math_transform_to_matrix(Transform transform);