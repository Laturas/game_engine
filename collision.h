#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

typedef enum LayerMask {
	MASK_NO_COLLISIONS   = 0,
	MASK_PLAYER          = (1 << 0),
	MASK_STATIC_GEOMETRY = (1 << 1),
	MASK_ENEMIES         = (1 << 2),

	MASK_COUNT,
	MASK_ALL             = -1,
} LayerMask;

typedef struct TriangleCollider {
	LayerMask mask;
	int entity_id;

	Vector3 vert_1;
	Vector3 vert_2;
	Vector3 vert_3;
} TriangleCollider;

typedef struct TriangleColliderArray {
	int length;
	TriangleCollider* colliders;
} TriangleColliderArray;

typedef struct ColliderColumnList {
	struct ColliderColumnList* next;
	struct TriangleCollider* collider;
} ColliderColumnList;

/**
 * Represents one unsorted vertical cell of colliders
 */
typedef struct SpaceCell {
	ColliderColumnList* list; 
} SpaceCell;

typedef struct SpacialHash {
	f32 cell_width;
	int x_axis_cell_count;
	int z_axis_cell_count;
	BoundingBox world_bounding_box;
	struct SpaceCell* cells;
} SpacialHash;

typedef struct RaycastHit {
	i64 entity_id;
	Vector3 point;
	TriangleCollider* collider;
} RaycastHit;

/* Default width of cells in the spacial hash */
#define DEFAULT_CELL_WIDTH 3.0f

/* Returns the bounding box for all active colliders. Returns an empty box (all zero) if there are no active colliders. */
BoundingBox collision_get_world_bounding_box(TriangleColliderArray static_colliders);

/* Inserts an array of triangle colliders into a spacial hash. Allocates internal spacial_hash structure into the collider data arena. */
void collision_spacial_hash_insert_array(Arena* collider_data_arena, SpacialHash* spacial_hash, TriangleColliderArray collider_array);

/* Constructs the spacial hash for all colliders */
SpacialHash collision_spacial_hash_create(Arena* collider_data_arena, TriangleColliderArray static_colliders);

RaycastHit collision_raycast(
	const SpacialHash* spacial_hash,
	LayerMask layer_mask,
	Vector3 start_point,
	Vector3 direction,
	float raycast_length
);