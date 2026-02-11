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

/**
 * Afterhours uses what's best described as an entity component system (ECS)
 * 
 * Many components only exist for the duration of a frame and are completley rebuilt from scratch each frame (collider, mesh, etc)
 * However, some things need to be persistent like the transform.
 * 
 * Idk where I'm going with this, it's still WIP
 */
typedef struct Entity {
    Transform transform;
} Entity;

typedef enum ModelID {
	MODEL_NONE = 0,
	MODEL_BOX,
	MODEL_TORUS,

	MODEL_ID_COUNT
} ModelID;

/**
 * StaticObjects are entirely built from scratch each frame.
 */
typedef struct StaticObject {
	Transform transform;
	LayerMask layer;
	ModelID id;
} StaticObject;

typedef struct StaticObjectArray {
	StaticObject* objects;
	int len;
} StaticObjectArray;

Matrix TransformToMatrix(Transform transform) {
	/* Extract rotation basis */
	Vector3 x = Vector3RotateByQuaternion(VECTOR3_UP, transform.rotation);
	Vector3 y = Vector3RotateByQuaternion(VECTOR3_RIGHT, transform.rotation);
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

/**
 * Handles things like terrain and other objects that do not change during their lifetime.
 */
TriangleColliderArray static_object_loop(Arena* collider_data_arena, StaticObjectArray static_objects, const Model* const model_prefabs) {
	TriangleColliderArray tri_array = {0};

	for (int i = 0; i < static_objects.len; i++) {
		StaticObject object = static_objects.objects[i];
		for (int mesh_index = 0; mesh_index < model_prefabs[object.id].meshCount; mesh_index++) {
			Mesh mesh = model_prefabs[object.id].meshes[mesh_index];
			int total_tris = mesh.vertexCount / 3;
			TriangleCollider* tris = arena_alloc(collider_data_arena, sizeof(TriangleCollider) * total_tris);

			for (int j = 0; j < total_tris; j += 1) {
				Matrix t_matrix = TransformToMatrix(object.transform);
				int stride = 3;
				Vector3 v1 = {
					mesh.vertices[stride * (j * 3 + 0) + 0],
					mesh.vertices[stride * (j * 3 + 0) + 1],
					mesh.vertices[stride * (j * 3 + 0) + 2],
				};

				Vector3 v2 = {
					mesh.vertices[stride * (j * 3 + 1) + 0],
					mesh.vertices[stride * (j * 3 + 1) + 1],
					mesh.vertices[stride * (j * 3 + 1) + 2],
				};

				Vector3 v3 = {
					mesh.vertices[stride * (j * 3 + 2) + 0],
					mesh.vertices[stride * (j * 3 + 2) + 1],
					mesh.vertices[stride * (j * 3 + 2) + 2],
				};
				v1 = Vector3Transform(v1, t_matrix);
				v2 = Vector3Transform(v2, t_matrix);
				v3 = Vector3Transform(v3, t_matrix);

				/* Currently, colliders lack support for rotation/scaling */
				tris[j] = (TriangleCollider) {
					.mask = MASK_STATIC_GEOMETRY,
					.vert_1 = v1,
					.vert_2 = v2,
					.vert_3 = v3,
				};

				if (tri_array.colliders == NULL) {
					tri_array.colliders = tris;
				}
				tri_array.length++;
			}
		}
	}
	return tri_array;
}