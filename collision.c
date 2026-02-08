/**
 * Afterhours uses a Spacial Hashing collision system. The system makes a few underlying assumptions:
 * - There are not a huge number of colliders along a vertical column
 * - Colliders do not handle special interactions (hurtboxes, dialogue, etc)
 * - We don't need full rigidbody physics.
 * General outline of the system:
 * - The spacial hash will be rebuilt every frame from scratch (for safety reasons)
 * - The world will be divided into a grid of vertical cells, then every collider will be added to each list corresponding to the vertical cells it overlaps
 * - The only collider primitive that exists is a triangle, and all other colliders are derived from combinations of these
 */
#ifndef AFTERHOURS
	#include "afterhours.h"
#endif

typedef int LayerMask;

#define MASK_NO_COLLISIONS (LayerMask)0
#define MASK_PLAYER (LayerMask)(1 << 0)
#define MASK_STATIC_GEOMETRY (LayerMask)(1 << 1)
#define MASK_ENEMIES (LayerMask)(1 << 2)

typedef struct TriangleCollider {
	LayerMask mask;
	f32 bounding_sphere_radius;
	Vector3 bounding_sphere_center;

	Vector3 vert_1;
	Vector3 vert_2;
	Vector3 vert_3;
} TriangleCollider;

typedef struct TriangleColliderArray {
	int length;
	TriangleCollider* colliders;
} TriangleColliderArray;

struct ColliderColumnList {
	struct ColliderColumnList* next;
	struct TriangleCollider* collider;
};

/**
 * Represents one unsorted vertical cell of colliders
 */
typedef struct SpaceCell {
	struct ColliderColumnList* list; 
} SpaceCell;

typedef struct SpacialHash {
	BoundingBox world_bounding_box;
	f32 cell_width;
	struct SpaceCell* cells;
} SpacialHash;

/* Default width of cells in the spacial hash */
#define DEFAULT_CELL_WIDTH 10.0f

/**
* Returns the bounding box for all active colliders.
*
* Returns an empty box (all zero) if there are no active colliders.
*/
BoundingBox get_world_bounding_box(TriangleColliderArray static_colliders, TriangleColliderArray dynamic_colliders) {
	BoundingBox world_bounding_box = {0};

	for (int i = 0; i < static_colliders.length; i++) {
		TriangleCollider collider = static_colliders.colliders[i];

		/* Vertex 1 */
		if (collider.vert_1.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_1.x; }
		if (collider.vert_1.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_1.x; }

		if (collider.vert_1.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_1.y; }
		if (collider.vert_1.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_1.y; }

		if (collider.vert_1.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_1.z; }
		if (collider.vert_1.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_1.z; }

		/* Vertex 2 */
		if (collider.vert_2.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_2.x; }
		if (collider.vert_2.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_2.x; }

		if (collider.vert_2.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_2.y; }
		if (collider.vert_2.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_2.y; }

		if (collider.vert_2.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_2.z; }
		if (collider.vert_2.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_2.z; }

		/* Vertex 3 */
		if (collider.vert_3.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_3.x; }
		if (collider.vert_3.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_3.x; }

		if (collider.vert_3.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_3.y; }
		if (collider.vert_3.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_3.y; }

		if (collider.vert_3.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_3.z; }
		if (collider.vert_3.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_3.z; }
	}

	for (int i = 0; i < dynamic_colliders.length; i++) {
		TriangleCollider collider = dynamic_colliders.colliders[i];

		/* Vertex 1 */
		if (collider.vert_1.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_1.x; }
		if (collider.vert_1.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_1.x; }

		if (collider.vert_1.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_1.y; }
		if (collider.vert_1.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_1.y; }

		if (collider.vert_1.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_1.z; }
		if (collider.vert_1.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_1.z; }

		/* Vertex 2 */
		if (collider.vert_2.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_2.x; }
		if (collider.vert_2.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_2.x; }

		if (collider.vert_2.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_2.y; }
		if (collider.vert_2.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_2.y; }

		if (collider.vert_2.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_2.z; }
		if (collider.vert_2.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_2.z; }

		/* Vertex 3 */
		if (collider.vert_3.x > world_bounding_box.max.x) { world_bounding_box.max.x = collider.vert_3.x; }
		if (collider.vert_3.x < world_bounding_box.min.x) { world_bounding_box.min.x = collider.vert_3.x; }

		if (collider.vert_3.y > world_bounding_box.max.y) { world_bounding_box.max.y = collider.vert_3.y; }
		if (collider.vert_3.y < world_bounding_box.min.y) { world_bounding_box.min.y = collider.vert_3.y; }

		if (collider.vert_3.z > world_bounding_box.max.z) { world_bounding_box.max.z = collider.vert_3.z; }
		if (collider.vert_3.z < world_bounding_box.min.z) { world_bounding_box.min.z = collider.vert_3.z; }
	}

	return world_bounding_box;
}

/**
* Constructs the spacial hash for all colliders
*/
void collider_loop(Arena* collider_data_arena, TriangleColliderArray static_colliders, TriangleColliderArray dynamic_colliders) {
	SpacialHash* spacial_hash = arena_alloc(collider_data_arena, sizeof(*spacial_hash));
	spacial_hash->cell_width = DEFAULT_CELL_WIDTH;
	spacial_hash->world_bounding_box = get_world_bounding_box(static_colliders, dynamic_colliders);

	u32 x_axis_cell_count = (
		spacial_hash->world_bounding_box.max.x - spacial_hash->world_bounding_box.min.x
	) / DEFAULT_CELL_WIDTH;
}