#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

/**
* Todo: ModelID is going to eventually be a hash based on file name of the model.
*
* Zero will always be a special value that indicates "no model".
*/
typedef enum ModelID {
	MODEL_NONE = 0,
	MODEL_BOX,
	MODEL_TORUS,

	MODEL_ID_COUNT
} ModelID;

/**
* This is for any and all data that is needed only for the editor.
*
* For example, entities need to have colliders in the editor so we can select them
* even if they don't have any in the actual game.
*/
typedef struct EditorEntityData {
	ModelID collider_model_id;
	ModelID visible_model_id;
} EditorEntityData;

/**
 * Afterhours uses what's best described as an entity component system (ECS)
 * 
 * Many components only exist for the duration of a frame and are completley rebuilt from scratch each frame (collider, mesh, etc)
 * However, some things need to be persistent like the transform.
 * 
 * The fundamental goal: rebuild as much as possible from scratch every frame.
 * NEVER hold an entity pointer past the end of a frame.
 *
 * All entities will be in a big array, indexed into via ID. Everything is an entity. Even static objects.
 * (particles won't be, but they're special)
 */
typedef struct Entity {
	int entity_type;
	Transform transform;
	
	ModelID collider_model_id;
	ModelID visible_model_id;

	EditorEntityData editor_data;
} Entity;

/**
 * StaticObjects are entirely built from scratch each frame.
 *
 * TODO: Do we actually want this to exist?
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

/* Handles things like terrain and other objects that do not change during their lifetime. Primarily generates the colliders. */
TriangleColliderArray static_object_loop(Arena* collider_data_arena, StaticObjectArray static_objects, const Model* model_prefabs);