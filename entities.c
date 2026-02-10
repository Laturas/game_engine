#pragma once

#ifndef AFTERHOURS
	#include "afterhours.h"
#endif

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

typedef enum StaticObjectID {
	SOBJ_NONE = 0,
	SOBJ_BOX,

	STATIC_OBJECT_TYPE_COUNT
} StaticObjectID;

/**
 * StaticObjects are entirely built from scratch each frame.
 */
typedef struct StaticObject {
	Transform transform;
	LayerMask layer;
	StaticObjectID id;
} StaticObject;

typedef struct StaticObjectArray {
	StaticObject* objects;
	int len;
} StaticObjectArray;

/**
 * Handles things like terrain and other objects that do not change during their lifetime.
 */
TriangleColliderArray static_object_loop(StaticObjectArray static_objects, Arena* collider_data_arena) {
	TriangleColliderArray* tri_array = arena_alloc(collider_data_arena, sizeof(*tri_array));

	for (int i = 0; i < static_objects.len; i++) {
		StaticObject object = static_objects.objects[i];
		switch (object.id) {
			case SOBJ_NONE: break;

			case SOBJ_BOX: {
				// fprintf(stderr, "Attempt draw cube\n");
				Model cube_model = LoadModel("assets/models/Cube.obj");
				Vector3 position = object.transform.translation;

				DrawModel(cube_model, position, 1.0f, WHITE);    // Draw animated model
			} break;

			default: {
				ASSERT(("Support for this static object type not handled", 0));
			} break;
		}
	}
}