#pragma once

#ifndef AFTERHOURS
	#include "afterhours.h"
#endif

#define VECTOR3_ZERO (Vector3){ 0.0f, 0.0f, 0.0f }
#define VECTOR3_UP (Vector3){ 0.0f, 1.0f, 0.0f }
#define VECTOR3_DOWN (Vector3){ 0.0f, -1.0f, 0.0f }
#define VECTOR3_FORWARD (Vector3){ 0.0f, 0.0f, 1.0f }
#define VECTOR3_BACKWARD (Vector3){ 0.0f, 0.0f, -1.0f }
#define VECTOR3_RIGHT (Vector3){ 1.0f, 0.0f, 0.0f }
#define VECTOR3_LEFT (Vector3){ -1.0f, 0.0f, 0.0f }

#define EPSILON 0.0001f

#define MIN3(a,b,c) (((a) < (b)) \
	? ((a) < (c)) ? (a) : (c) \
	: ((b) < (c)) ? (b) : (c))

#define MAX3(a,b,c) (((a) > (b)) \
	? ((a) > (c)) ? (a) : (c) \
	: ((b) > (c)) ? (b) : (c))

/**
 * Branchless absolute value for floats.
 * Clears the sign bit
 */
f32 f32_abs(f32 value) {
	union {
        f32 f;
        u32 i;
    } u;

    u.f = value;
    u.i &= 0x7FFFFFFF;
    return u.f;
}

/**
* A short, fast, branchless version of floor, rounding down to the nearest integer.
*/
f32 f32_floor(f32 value) {
	/* Truncates it towards zero */
	int i = (int)value;
	/* Subtracts 1 if it was negative */
	return (float)(i - (i > value));
}

/**
* A short, fast, branchless version of ceiling, rounding up to the nearest integer.
*/
f32 f32_ceiling(f32 value) {
	/* Truncates it towards zero */
	int i = (int)value;
	/* Adds 1 if it was positive */
	return (float)(i + (i <= value));
}

/**
 * Returns whether a line defined by (x_0 + at, y_0 + bt, z_0 + ct) intersects with a given triangle
 */
bool line_intersects_triangle(
	float x_0, float y_0, float z_0, /* triangle point 1 */
	float x_1, float y_1, float z_1, /* triangle point 2 */
	float x_2, float y_2, float z_2, /* triangle point 3 */

	/* line = (x_0 + at, y_0 + bt, z_0 + ct) */
	float line_x_0, float a,
	float line_y_0, float b,
	float line_z_0, float c
) {
	/* Triangle edges (for cross product) */
	float e1x = x_1 - x_0;
	float e1y = y_1 - y_0;
	float e1z = z_1 - z_0;

	float e2x = x_2 - x_0;
	float e2y = y_2 - y_0;
	float e2z = z_2 - z_0;

	/* Triangle normal (via cross product of two edges) */
	float nx = e1y * e2z - e1z * e2y;
	float ny = e1z * e2x - e1x * e2z;
	float nz = e1x * e2y - e1y * e2x;

	f32 denom = (nx * a) + (ny * b) + (nz * c);

	/* denom = 0 means paralell to triangle. */
	if (f32_abs(denom) < EPSILON) { return false; }

	float px = x_0 - line_x_0;
	float py = y_0 - line_y_0;
	float pz = z_0 - line_z_0;

	float t = (nx * px + ny * py + nz * pz) / denom;

	float plane_intersect_x = line_x_0 + (a * t);
	float plane_intersect_y = line_y_0 + (b * t);
	float plane_intersect_z = line_z_0 + (c * t);

	/* Testing intersection via barycentric test */
	float v0x = x_2 - x_0;
	float v0y = y_2 - y_0;
	float v0z = z_2 - z_0;

	float v1x = x_1 - x_0;
	float v1y = y_1 - y_0;
	float v1z = z_1 - z_0;

	float v2x = plane_intersect_x - x_0;
	float v2y = plane_intersect_y - y_0;
	float v2z = plane_intersect_z - z_0;

	/* Dot products: We agree! */
	float dot00 = (v0x * v0x) + (v0y * v0y) + (v0z * v0z);
	float dot01 = (v0x * v1x) + (v0y * v1y) + (v0z * v1z);
	float dot02 = (v0x * v2x) + (v0y * v2y) + (v0z * v2z);
	float dot11 = (v1x * v1x) + (v1y * v1y) + (v1z * v1z);
	float dot12 = (v1x * v2x) + (v1y * v2y) + (v1z * v2z);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

	float u = ((dot11 * dot02) - (dot01 * dot12)) * invDenom;
	float v = ((dot00 * dot12) - (dot01 * dot02)) * invDenom;

	// Check if point is inside triangle
	return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
}

Matrix transform_to_matrix(Transform transform) {
	/* Extract rotation basis */
	Vector3 x = Vector3RotateByQuaternion(VECTOR3_RIGHT, transform.rotation);
	Vector3 y = Vector3RotateByQuaternion(VECTOR3_UP, transform.rotation);
	Vector3 z = Vector3RotateByQuaternion(VECTOR3_FORWARD, transform.rotation);
	
	/* Scale basis vectors */
	x = Vector3Scale(x, transform.scale.x);
	y = Vector3Scale(y, transform.scale.y);
	z = Vector3Scale(z, transform.scale.z);
	
	Vector3 t = transform.translation;
	
	return (Matrix){
		x.x, y.x, z.x, t.x,
		x.y, y.y, z.y, t.y,
		x.z, y.z, z.z, t.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}