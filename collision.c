/**
 * Afterhours uses a Spacial Hashing collision system. The system makes a few underlying assumptions:
 * - There are not a huge number of colliders along a vertical column
 * - Colliders do not handle special interactions (hurtboxes, dialogue, etc)
 * - We don't need full rigidbody physics.
 * 
 * General outline of the system:
 * - The spacial hash will be rebuilt every frame from scratch (for safety reasons)
 * - The world will be divided into a grid of vertical cells, then every collider will be added to each list corresponding to the vertical cells it overlaps
 * - The only collider primitive that exists is a triangle, and all other colliders are derived from combinations of these
 */

#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

/**
* Returns the bounding box for all active colliders.
*
* Returns an empty box (all zero) if there are no active colliders.
*/
BoundingBox collision_get_world_bounding_box(TriangleColliderArray colliders) {
	BoundingBox world_bounding_box = {0};

	for (int i = 0; i < colliders.length; i++) {
		TriangleCollider collider = colliders.colliders[i];

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

RaycastHit collision_raycast(
	const SpacialHash* spacial_hash,
	LayerMask layer_mask,
	Vector3 start_point,
	Vector3 direction,
	float raycast_length
) {
	RaycastHit rc_hit = (RaycastHit) {
		.collider = NULL,
		.entity_id = 0,
		.point = VECTOR3_INFINITY,
	};
	f32 current_displacement = INFINITY;

	/* TODO: Optimize this to actually use the spacial hash. Right now we just brute force. */
	int cell_count = spacial_hash->x_axis_cell_count * spacial_hash->z_axis_cell_count;

	for (int i = 0; i < cell_count; i++) {
		ColliderColumnList* list = spacial_hash->cells[i].list;

		while (list != NULL) {
			TriangleCollider col = *list->collider;

			if (list->collider->mask & layer_mask) {
				Vector3 intersection_point = math_line_triangle_intersection(
					col.vert_1, col.vert_2, col.vert_3,
					start_point,
					direction.x, direction.y, direction.z
				);
				Vector3 displacement_vec = Vector3Subtract(intersection_point, start_point);
				f32 displacement = Vector3Length(displacement_vec);

				if (displacement < raycast_length && displacement < current_displacement) {
					rc_hit.collider = list->collider;
					rc_hit.point = intersection_point;
					rc_hit.entity_id = col.entity_id;

					current_displacement = displacement;
				}
			}
			list = list->next;
		}
	}
	return rc_hit;
}

void collision_spacial_hash_insert_array(Arena* collider_data_arena, SpacialHash* spacial_hash, TriangleColliderArray collider_array) {
	for (int i = 0; i < collider_array.length; i++) {
		/* Inserts each collider triangle into the spacial hash */
		TriangleCollider tri = collider_array.colliders[i];

		f32 cell_width = spacial_hash->cell_width;

		f32 x_0 = tri.vert_1.x; f32 z_0 = tri.vert_1.z;
		f32 x_1 = tri.vert_2.x; f32 z_1 = tri.vert_2.z;
		f32 x_2 = tri.vert_3.x; f32 z_2 = tri.vert_3.z;

		f32 min_x = MIN3(x_0, x_1, x_2); f32 min_z = MIN3(z_0, z_1, z_2);
		f32 max_x = MAX3(x_0, x_1, x_2); f32 max_z = MAX3(z_0, z_1, z_2);

		/* We now have a box defining the region where we will probe cells. We now need to convert to integers we can use to index */

		int min_cell_x; int min_cell_z; {
			f32 min_cell_x_f32_no_offset = (min_x - spacial_hash->world_bounding_box.min.x);
			f32 min_cell_x_f32_no_scale  = min_cell_x_f32_no_offset / cell_width;
			
			f32 min_cell_z_f32_no_offset = (min_z - spacial_hash->world_bounding_box.min.z);
			f32 min_cell_z_f32_no_scale  = min_cell_z_f32_no_offset / cell_width;
			
			min_cell_x = (int)math_f32_floor(min_cell_x_f32_no_scale);
			min_cell_z = (int)math_f32_floor(min_cell_z_f32_no_scale);
		}


		int max_cell_x; int max_cell_z; {
			/* We still subtract from the minimum here, because we want the position relative to the minimum cell to index into the array */
			f32 max_cell_x_f32_no_offset = (max_x - spacial_hash->world_bounding_box.min.x);
			f32 max_cell_x_f32_no_scale  = max_cell_x_f32_no_offset / cell_width;
	
			f32 max_cell_z_f32_no_offset = (max_z - spacial_hash->world_bounding_box.min.z);
			f32 max_cell_z_f32_no_scale  = max_cell_z_f32_no_offset / cell_width;
	
			max_cell_x = (int)math_f32_floor(max_cell_x_f32_no_scale);
			max_cell_z = (int)math_f32_floor(max_cell_z_f32_no_scale);
		}

		/* Extra tests */ {
			int max_x = spacial_hash->x_axis_cell_count;
			int max_z = spacial_hash->z_axis_cell_count;

			if (NEVER(min_cell_x < 0 || min_cell_z < 0)) { continue; }
			if (NEVER(max_cell_x >= max_x || max_cell_z >= max_z)) { continue; }
		}


		/* Iterate the cells, test if they're within the triangle, insert. */
		/* TODO: Actually insert a triangle test. */
		for (int z = min_cell_z; z <= max_cell_z; z++) {
			for (int x = min_cell_x; x <= max_cell_x; x++) {
				ColliderColumnList* list_node = arena_alloc(collider_data_arena, sizeof(*list_node));

				/* TODO: Make relative pointer */
				list_node->collider = &collider_array.colliders[i];
				list_node->next = spacial_hash->cells[(spacial_hash->x_axis_cell_count * z) + x].list;

				spacial_hash->cells[(spacial_hash->x_axis_cell_count * z) + x].list = list_node;
			}
		}
	}
}

/**
* Constructs the spacial hash for all colliders
*/
SpacialHash collision_spacial_hash_create(Arena* collider_data_arena, TriangleColliderArray static_colliders) {
	BoundingBox world_bound = collision_get_world_bounding_box(static_colliders);
	SpacialHash spacial_hash = (SpacialHash) {
		.cell_width = DEFAULT_CELL_WIDTH,
		.world_bounding_box = world_bound,
		.cells = NULL,
		.x_axis_cell_count = ((world_bound.max.x - world_bound.min.x) / DEFAULT_CELL_WIDTH) + 1,
		.z_axis_cell_count = ((world_bound.max.z - world_bound.min.z) / DEFAULT_CELL_WIDTH) + 1
	};

	spacial_hash.cells = arena_alloc(collider_data_arena, sizeof(*spacial_hash.cells) * spacial_hash.x_axis_cell_count * spacial_hash.z_axis_cell_count);

	for (int z = 0; z < spacial_hash.z_axis_cell_count; z++) {
		for (int x = 0; x < spacial_hash.x_axis_cell_count; x++) {
			spacial_hash.cells[(spacial_hash.x_axis_cell_count * z) + x] = (SpaceCell) { .list = NULL };
		}
	}

	collision_spacial_hash_insert_array(collider_data_arena, &spacial_hash, static_colliders);

	return spacial_hash;
}