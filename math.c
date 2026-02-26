#pragma once

#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

/**
 * Branchless absolute value for floats.
 * Clears the sign bit
 */
f32 fn math_f32_abs(f32 value) {
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
f32 fn math_f32_floor(f32 value) {
	/* Truncates it towards zero */
	int i = (int)value;
	/* Subtracts 1 if it was negative */
	return (float)(i - (i > value));
}

/**
* A short, fast, branchless version of ceiling, rounding up to the nearest integer.
*/
f32 fn math_f32_ceiling(f32 value) {
	/* Truncates it towards zero */
	int i = (int)value;
	/* Adds 1 if it was positive */
	return (float)(i + (i <= value));
}

Vector3 fn math_triplane_line_intersection(
	Vector3 tri_point_1,
	Vector3 tri_point_2,
	Vector3 tri_point_3,

	Vector3 line_0, float a, float b, float c
) {
	Vector3 return_vec = VECTOR3_INFINITY;

	float e1x = tri_point_2.x - tri_point_1.x;
	float e1y = tri_point_2.y - tri_point_1.y;
	float e1z = tri_point_2.z - tri_point_1.z;

	float e2x = tri_point_3.x - tri_point_1.x;
	float e2y = tri_point_3.y - tri_point_1.y;
	float e2z = tri_point_3.z - tri_point_1.z;

	/* Triangle normal (via cross product of two edges) */
	float nx = e1y * e2z - e1z * e2y;
	float ny = e1z * e2x - e1x * e2z;
	float nz = e1x * e2y - e1y * e2x;

	f32 denom = (nx * a) + (ny * b) + (nz * c);

	/* denom = 0 means paralell to triangle. */
	if (math_f32_abs(denom) >= EPSILON) {
		float px = tri_point_1.x - line_0.x;
		float py = tri_point_1.y - line_0.y;
		float pz = tri_point_1.z - line_0.z;
	
		float t = (nx * px + ny * py + nz * pz) / denom;

		return_vec.x = line_0.x + (a * t);
		return_vec.y = line_0.y + (a * t);
		return_vec.z = line_0.z + (a * t);
	}

	return return_vec;
}

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
) {
	Vector3 intersection = math_triplane_line_intersection(
		tri_point_1, tri_point_2, tri_point_3,
		line_0, a, b, c
	);
	if (Vector3Length(intersection) == INFINITY) { return intersection; }

	/* Testing intersection via barycentric test */
	float v0x = tri_point_3.x - tri_point_1.x;
	float v0y = tri_point_3.y - tri_point_1.y;
	float v0z = tri_point_3.z - tri_point_1.z;

	float v1x = tri_point_2.x - tri_point_1.x;
	float v1y = tri_point_2.y - tri_point_1.y;
	float v1z = tri_point_2.z - tri_point_1.z;

	float v2x = intersection.x - tri_point_1.x;
	float v2y = intersection.y - tri_point_1.y;
	float v2z = intersection.z - tri_point_1.z;

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
	if ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f)) {
		return intersection;
	} else {
		return VECTOR3_INFINITY;
	}
}

Matrix fn math_transform_to_matrix(Transform transform) {
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
