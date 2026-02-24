#pragma once

#ifndef AFTERHOURS_H
	#include "afterhours.h"
#endif

/**
 * Handles things like terrain and other objects that do not change during their lifetime.
 */
TriangleColliderArray static_object_loop(Arena* collider_data_arena, StaticObjectArray static_objects, const Model* model_prefabs) {
	TriangleColliderArray tri_array = {0};

	for (int i = 0; i < static_objects.len; i++) {
		StaticObject object = static_objects.objects[i];
		for (int mesh_index = 0; mesh_index < model_prefabs[object.id].meshCount; mesh_index++) {
			Mesh mesh = model_prefabs[object.id].meshes[mesh_index];
			int total_tris = mesh.vertexCount / 3;
			TriangleCollider* tris = arena_alloc(collider_data_arena, sizeof(TriangleCollider) * total_tris);

			for (int j = 0; j < total_tris; j += 1) {
				Matrix t_matrix = math_transform_to_matrix(object.transform);
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
					/* TODO: Introduce a better way to rep the entity id. */
					.entity_id = i,
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