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


// #ifndef AFTERHOURS
// 	#include "afterhours.h"
// #endif

// global Arena static_colliders;
// global Arena dynamic_colliders;
// global Arena collider_octree_arena;

// typedef enum ColliderType {
// 	COLLIDER_NONE,
// 	COLLIDER_SPHERE,
// 	COLLIDER_BOX,
// 	COLLIDER_TRIANGLE,
// 	COLLIDER_CAPSULE,
// } ColliderType;

// #define MASK_NO_COLLISIONS 0
// #define MASK_PLAYER (1 << 0)
// #define MASK_STATIC_GEOMETRY (1 << 1)
// #define MASK_ENEMIES (1 << 2)

// typedef int LayerMask;

// typedef struct SphereCollider {
// 	Vector3 position;
// 	f32 radius;
// } SphereCollider;

// typedef struct BoxCollider {
// 	Vector3 position;
// 	Vector3 dimensions;
// } BoxCollider;

// typedef struct TriangleCollider {
// 	Vector3 vert_1;
// 	Vector3 vert_2;
// 	Vector3 vert_3;
// } TriangleCollider;

// typedef struct CapsuleCollider {
// 	Vector3 vert_1;
// 	Vector3 vert_2;
// 	f32 radius;
// } CapsuleCollider;

// typedef struct NullCollider {
// 	u64 none;
// } NullCollider;

// union ColliderData {
// 	SphereCollider sphere_type;
// 	BoxCollider box_type;
// 	TriangleCollider triangle_type;
// 	CapsuleCollider capsule_type;
// };

// typedef struct Collider {
// 	LayerMask mask;
// 	ColliderType collider_type;
// 	union ColliderData data_union;
// } Collider;


// #define OCTREE_NODE_NONE 0
// #define OCTREE_NODE_STATIC_COLLIDER 1
// #define OCTREE_NODE_DYNAMIC_COLLIDER 2
// #define OCTREE_NODE_SUBTREE 3

// /**
//  * 
//  */
// typedef struct ColliderOctreeNode {
// 	Vector3 position; /* Position of the cube corner */
// 	f32 size; /* Width of the cube */
// 	u8 tag[8];
// 	u64 node[8];
// } ColliderOctreeNode;

// ColliderOctreeNode* initialize_octree(Arena* octree_arena, Vector3 world_bounding_box_start, f32 bounding_box_size) {
// 	ColliderOctreeNode* tree_parent_node = arena_alloc(octree_arena, sizeof(ColliderOctreeNode));
// 	if (tree_parent_node == NULL) return NULL;

// 	tree_parent_node->position = world_bounding_box_start;
// 	tree_parent_node->size = bounding_box_size;
// 	for (int i = 0; i < 8; i++) {
// 		tree_parent_node->tag[i] = OCTREE_NODE_NONE;
// 		tree_parent_node->node[i] = 0;
// 	}
// 	return tree_parent_node;
// }

// struct ColliderArray {
// 	Collider* colliders;
// 	u64 length;
// };

// // void insert_collider_into_octree(Arena* octree_arena, ColliderOctreeNode* octree_node, Collider collider) {
// // 	for (int i = 0; i < 8; i++) {

// // 		switch (collider.collider_type) {
// // 			case COLLIDER_NONE: { return; }
// // 			case COLLIDER_SPHERE: {
				
// // 			}
// // 			break;
// // 			case COLLIDER_CAPSULE: {
	
// // 			}
// // 			break;
// // 			case COLLIDER_BOX: {
				
// // 			}
// // 			break;
// // 			case COLLIDER_TRIANGLE: {
	
// // 			}
// // 			break;
// // 		}
// // 	}
// // }

// void collision_loop(struct ColliderArray static_colliders,
// 	Arena* dynamic_colliders,
// 	Arena* collider_octree_arena) {
// 	arena_restore(collider_octree_arena, 0);

// 	/* TODO */
// 	// Vector3 start_pos = (Vector3) {0.0f, 0.0f, 0.0f};
// 	// f32 world_width = 1000.0f;
// 	// ColliderOctreeNode* parent = initialize_octree(collider_octree_arena, start_pos, world_width);
	
// 	// for (u64 i = 0; i < static_colliders.length; i++) {
// 	// 	insert_collider_into_octree(collider_octree_arena, parent, static_colliders.colliders[i]);
// 	// }
// }

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

struct TriangleColliderArray {
	int length;
	TriangleCollider* colliders;
};

struct ElementList {
	struct ElementList* next;
	struct TriangleCollider* next;
};

/**
 * Represents one unsorted vertical cell of colliders
 */
struct SpaceCell {
	struct ElementList* 
};

struct SpacialHash {
	f32 x_width;
	f32 z_width;
	f32 cell_width;
	struct SpaceCell* cells;
};

void collider_loop(
	Arena* collider_data_arena,
	struct TriangleColliderArray static_colliders,
	struct TriangleColliderArray dynamic_colliders
) {
	
}