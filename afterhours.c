#pragma once

#include "afterhours.h"

#include "math.c"
#include "hash_map.c"

#include "collision.c"
#include "entities.c"
#include "ui.c"
#include "immediate_ui.c"

#include "ui_experiment.c"


/* Leave this undefined. This is a region. */
#ifndef REGION_RAYLIB_CAMERA_FUNCTIONALITY
	/* Units per second */
	#define CAMERA_MOVE_SPEED		5.4f
	#define CAMERA_ROTATION_SPEED	1.0f
	#define CAMERA_PAN_SPEED		1.0f

	#define CAMERA_MOUSE_MOVE_SENSITIVITY	0.003f

	/* Camera orbital speed in CAMERA_ORBITAL mode. Units in radians/second */
	#define CAMERA_ORBITAL_SPEED	0.5f

	void CameraMoveToTarget(Camera *camera, float delta);
	void CameraMoveUp(Camera *camera, float distance);
	void CameraMoveRight(Camera *camera, float distance, bool moveInWorldPlane);
	Vector3 GetCameraForward(Camera *camera);
	Vector3 GetCameraUp(Camera *camera);
	Vector3 GetCameraRight(Camera *camera);
	void CameraPitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
	void CameraYaw(Camera *camera, float angle, bool rotateAroundTarget);
	void CameraRoll(Camera *camera, float angle);
	void CameraMoveForward(Camera *camera, float distance, bool moveInWorldPlane);
#endif

void update_editor_camera(Camera *camera) {
	Vector2 mousePositionDelta = GetMouseDelta();

	int mode = CAMERA_FREE;

	bool moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
	bool rotateAroundTarget = ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
	bool lockView = ((mode == CAMERA_FREE) || (mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
	bool rotateUp = false;

	// Camera speeds based on frame time
	float cameraMoveSpeed = CAMERA_MOVE_SPEED*GetFrameTime();
	float cameraRotationSpeed = CAMERA_ROTATION_SPEED*GetFrameTime();
	float cameraPanSpeed = CAMERA_PAN_SPEED*GetFrameTime();
	float cameraOrbitalSpeed = CAMERA_ORBITAL_SPEED*GetFrameTime();

	if (mode == CAMERA_CUSTOM) {}
	else if (mode == CAMERA_ORBITAL) {
		// Orbital can just orbit
		Matrix rotation = MatrixRotate(Vector3Normalize(camera->up), cameraOrbitalSpeed);
		Vector3 view = Vector3Subtract(camera->position, camera->target);
		view = Vector3Transform(view, rotation);
		camera->position = Vector3Add(camera->target, view);
	}
	else {
		// Camera rotation
		if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, -cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
		if (IsKeyDown(KEY_UP)) CameraPitch(camera, cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
		if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, -cameraRotationSpeed, rotateAroundTarget);
		if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, cameraRotationSpeed, rotateAroundTarget);
		if (IsKeyDown(KEY_Q)) CameraRoll(camera, -cameraRotationSpeed);
		if (IsKeyDown(KEY_E)) CameraRoll(camera, cameraRotationSpeed);

		// Camera movement
		// Camera pan (for CAMERA_FREE)
		if ((mode == CAMERA_FREE) && (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))) {
			const Vector2 mouseDelta = GetMouseDelta();
			if (mouseDelta.x > 0.0f) CameraMoveRight(camera, cameraPanSpeed, moveInWorldPlane);
			if (mouseDelta.x < 0.0f) CameraMoveRight(camera, -cameraPanSpeed, moveInWorldPlane);
			if (mouseDelta.y > 0.0f) CameraMoveUp(camera, -cameraPanSpeed);
			if (mouseDelta.y < 0.0f) CameraMoveUp(camera, cameraPanSpeed);
		}
		else {
			// Mouse support
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				CameraYaw(camera, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
				CameraPitch(camera, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
			}
		}

		// Keyboard support
		if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
		if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
		if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
		if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);

		if (mode == CAMERA_FREE) {
			if (IsKeyDown(KEY_SPACE)) CameraMoveUp(camera, cameraMoveSpeed);
			if (IsKeyDown(KEY_LEFT_SHIFT)) CameraMoveUp(camera, -cameraMoveSpeed);
		}
	}

	if ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL) || (mode == CAMERA_FREE))
	{
		// Zoom target distance
		CameraMoveToTarget(camera, -GetMouseWheelMove());
		if (IsKeyPressed(KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
		if (IsKeyPressed(KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
	}
}

Transform default_transform() {
	return (Transform) {
		.rotation = QuaternionIdentity(),
		.scale = (Vector3) {1.0f, 1.0f, 1.0f},
		.translation = (Vector3) {2.0f, 0.0f, 0.0f}
	};
}

void draw_model(ModelID model_id, const Model* model_prefabs, Transform model_transform) {
	if (model_id == MODEL_NONE) { return; }

	DrawModelEx(model_prefabs[model_id], model_transform.translation, VECTOR3_UP, 0.0f, model_transform.scale, WHITE);
}

void editor_draw_ui() {
	Arena strings_arena = {0};
	Arena ui_arena = {0};
	UICommandContext context = {0};
	UIRegionParameters params = {
		.background_color = SKYBLUE,
		.background_fade = 0.5f,
		.border_color = RAYWHITE,
		.border_fade = 0.9f,
		.horizontal_spacing = 15,
		.vertical_spacing = 15,
	};

	Rectangle screen_rect = { .height = GetScreenHeight(), .width = GetScreenWidth(), .x = 0.0f, .y = 0.0f };
	
	imui_region_begin(&ui_arena, &context, screen_rect, IMDIR_VERTICAL, (UIRegionParameters){.vertical_spacing = 15.0f, .horizontal_spacing = 15.0f});

		imui_draw_fps(&ui_arena, &context);

		Rectangle panel_rect = { .height = 320.0f, .width = 320.0f, .x = 15.0f, .y = 45.0f };

		imui_region_begin(&ui_arena, &context, panel_rect, IMDIR_VERTICAL, params);
			imui_draw_text(&ui_arena, &context, (String) {"Objects", sizeof("Objects")}, RAYWHITE, 1.0f, 16.0f);

			imui_draw_padding(&ui_arena, &context, 5);

			StringArray models = fs_get_files_in_dir(&strings_arena, (String) {"assets/models", sizeof("assets/models") - 1});
			String model_postfix = {.str = ".obj", .length = sizeof(".obj") - 1};

			for (int i = 0; i < models.len; i++) {
				String current = models.strings[i];

				if (string_ends_with(current, model_postfix)) {
					imui_draw_button(&ui_arena, &context,
						current,
						Fade(SKYBLUE, 0.5f),  /* Default color */
						Fade(BLUE, 0.5f),     /* Hover color */
						Fade(DARKBLUE, 0.5f), /* Click color */
						16.0f,                /* Font size */
						5.0f,                  /* Internal padding */
						(Vector2) { panel_rect.width - ((float)params.horizontal_spacing * 2), 0.0f}
					);
					imui_draw_padding(&ui_arena, &context, 5);
				}
			}
		imui_region_end(&ui_arena, &context);

	imui_region_end(&ui_arena, &context);

	imui_context_render(context);
	arena_free(&ui_arena);
	arena_free(&strings_arena);
}

void editor_loop(
	Camera*               main_camera,
	StaticObjectArray     static_objects,
	const Model*          model_prefabs,
	TriangleColliderArray optional_render_colliders,
	SpacialHash           optional_render_spacial_hash
) {
	update_editor_camera(main_camera);
	BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(*main_camera);
			for (int i = 0; i < static_objects.len; i++) {
				StaticObject current_object = static_objects.objects[i];
				draw_model(current_object.id, model_prefabs, current_object.transform);
			}

			for (int i = 0; i < optional_render_colliders.length; i++) {
				TriangleCollider tri = optional_render_colliders.colliders[i];
				DrawLine3D(tri.vert_1, tri.vert_2, LIME);
				DrawLine3D(tri.vert_2, tri.vert_3, LIME);
				DrawLine3D(tri.vert_3, tri.vert_1, LIME);
			}

			if (optional_render_spacial_hash.cells != NULL) {
				SpacialHash spacial_hash = optional_render_spacial_hash;

				for (int z = 0; z < spacial_hash.z_axis_cell_count; z++) {
					for (int x = 0; x < spacial_hash.x_axis_cell_count; x++) {
						if (spacial_hash.cells[(spacial_hash.x_axis_cell_count * z) + x].list != NULL) {
							Vector3 position = {
								.x = x * spacial_hash.cell_width + spacial_hash.world_bounding_box.min.x + (spacial_hash.cell_width / 2.0f),
								.y = 0,
								.z = z * spacial_hash.cell_width + spacial_hash.world_bounding_box.min.z + (spacial_hash.cell_width / 2.0f)
							};
		
							DrawCubeWires(position, spacial_hash.cell_width, 100.0f, spacial_hash.cell_width, LIGHTGRAY);
						}
					}
				}
				
				float world_bounding_box_width  = math_f32_abs(spacial_hash.world_bounding_box.max.x) + math_f32_abs(spacial_hash.world_bounding_box.min.x);
				float world_bounding_box_height = math_f32_abs(spacial_hash.world_bounding_box.max.y) + math_f32_abs(spacial_hash.world_bounding_box.min.y);
				float world_bounding_box_length = math_f32_abs(spacial_hash.world_bounding_box.max.z) + math_f32_abs(spacial_hash.world_bounding_box.min.z);

				Vector3 bounding_box_position = {
					.x = (spacial_hash.world_bounding_box.max.x + spacial_hash.world_bounding_box.min.x) / 2.0f,
					.y = (spacial_hash.world_bounding_box.max.y + spacial_hash.world_bounding_box.min.y) / 2.0f,
					.z = (spacial_hash.world_bounding_box.max.z + spacial_hash.world_bounding_box.min.z) / 2.0f,
				};
				DrawCubeWires(bounding_box_position, world_bounding_box_width, world_bounding_box_height, world_bounding_box_length, RED);
			}
		EndMode3D();

		#ifdef UNUSED
			draw_editor_ui();
		#endif

		#ifdef UNUSED
			editor_draw_ui();
		#endif

		test_example();
	EndDrawing();
}

#ifndef REGION_DONT_CARE
void update_game_camera(Camera* camera) {
	Vector2 mousePositionDelta = GetMouseDelta();

	#ifdef UNUSED
		bool moveInWorldPlane = true;
	#endif

	bool rotateAroundTarget = true;
	bool lockView = true;
	bool rotateUp = false;

	// Camera speeds based on frame time
	float cameraRotationSpeed = CAMERA_ROTATION_SPEED*GetFrameTime();

	#ifdef UNUSED
		float cameraMoveSpeed = CAMERA_MOVE_SPEED*GetFrameTime();
		float cameraPanSpeed = CAMERA_PAN_SPEED*GetFrameTime();
		float cameraOrbitalSpeed = CAMERA_ORBITAL_SPEED*GetFrameTime();
	#endif

	// Camera rotation
	if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, -cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
	if (IsKeyDown(KEY_UP)) CameraPitch(camera, cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
	if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, -cameraRotationSpeed, rotateAroundTarget);
	if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, cameraRotationSpeed, rotateAroundTarget);
	if (IsKeyDown(KEY_Q)) CameraRoll(camera, -cameraRotationSpeed);
	if (IsKeyDown(KEY_E)) CameraRoll(camera, cameraRotationSpeed);

	// Mouse support
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		CameraYaw(camera, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
		CameraPitch(camera, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
	}

	// Keyboard support
	if (IsKeyDown(KEY_W)) camera->position = Vector3Add(camera->target, VECTOR3_FORWARD);
	if (IsKeyDown(KEY_A)) camera->position = Vector3Add(camera->target, VECTOR3_LEFT);
	if (IsKeyDown(KEY_S)) camera->position = Vector3Add(camera->target, VECTOR3_BACKWARD);
	if (IsKeyDown(KEY_D)) camera->position = Vector3Add(camera->target, VECTOR3_RIGHT);

	CameraMoveToTarget(camera, -GetMouseWheelMove());
	if (IsKeyPressed(KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
	if (IsKeyPressed(KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
}

void main_game_loop(Camera* main_camera) {
	DrawCube(main_camera->target, 0.5f, 0.5f, 0.5f, PURPLE);
	DrawCubeWires(main_camera->target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
	update_game_camera(main_camera);

	BeginDrawing();
		DrawFPS(15, 15);

		ClearBackground(BLACK);

		BeginMode3D(*main_camera);

			// DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
			DrawCubeWires(VECTOR3_ZERO, 2.0f, 2.0f, 2.0f, MAROON);

			DrawGrid(10, 1.0f);

		EndMode3D();

		DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
		DrawRectangleLines( 10, 10, 320, 93, BLUE);

		DrawText("Free camera default controls:", 20, 20, 10, BLACK);
		DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, RAYWHITE);
		DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, RAYWHITE);
		DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, RAYWHITE);

	EndDrawing();
}
#endif

enum game_loop {
	GAMELOOP_GAME,
	GAMELOOP_EDITOR,
};

enum game_loop loop_mode;

void initialize_model(Model* model_prefab, ModelID model_to_load) {
	switch (model_to_load) {
		case MODEL_NONE: {
			*model_prefab = (Model) {0};
			return;
		} break;

		case MODEL_BOX: {
			*model_prefab = LoadModel("assets/models/Cube.obj");
		} break;

		case MODEL_TORUS: {
			*model_prefab = LoadModel("assets/models/torus.obj");
		} break;

		default: {
			ASSERT("Support for this model type not handled"?0:0);
		} break;
	}
}

void initialize_models(Model* model_prefabs, int model_count) {
	for (int i = 0; i < model_count; i++) {
		initialize_model(&(model_prefabs[i]), (ModelID)i);
	}
}

/**
* Static objects will be initialized on scene load. This is a temporary function for test purposes.
*/
StaticObjectArray test_initialize_static_objects(Arena* static_object_data) {
	int object_count = 3;
	StaticObject* object_array = arena_alloc(static_object_data, sizeof(*object_array) * object_count);

	object_array[0] = (StaticObject) {
		.id = MODEL_BOX,
		.layer = MASK_STATIC_GEOMETRY,
		.transform = default_transform(),
	};
	object_array[0].transform.translation = (Vector3) {2.0f, 0.0f, 0.0f};

	object_array[1] = (StaticObject) {
		.id = MODEL_BOX,
		.layer = MASK_STATIC_GEOMETRY,
		.transform = default_transform(),
	};
	
	object_array[1].transform.translation = (Vector3) {0.0f, 0.0f, -3.0f};
	object_array[1].transform.scale = (Vector3) {1.0f, 2.0f, 1.0f};
	
	object_array[2] = (StaticObject) {
		.id = MODEL_TORUS,
		.layer = MASK_STATIC_GEOMETRY,
		.transform = default_transform(),
	};

	object_array[2].transform.translation = (Vector3) {-15.0f,-3.0f,-3.0f,};

	return (StaticObjectArray) {
		.objects = object_array,
		.len = object_count
	};
}

int afterhours_main(void) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	InitWindow(screenWidth, screenHeight, "Afterengine");
	SetTargetFPS(60);
	SetExitKey(0); /* Disables ESC = exit */


	Camera3D main_camera = { 
		.position = (Vector3){ 10.0f, 10.0f, 10.0f },
		.target = VECTOR3_ZERO,
		.up = VECTOR3_UP,
		.fovy = 45.0f,
		.projection = CAMERA_PERSPECTIVE,
	};

	loop_mode = GAMELOOP_EDITOR;

	/* The arena brothers */
	Arena scene_arena = {0};         /* Stores all data related to the current scene. Static objects, lighting data, etc. Mainly things that don't change. */
	Arena collider_data_arena = {0}; /* Stores all collider data for the current frame. Gets reset every frame. */
	Arena model_data_arena = {0};    /* Stores all loadable models preloaded for future use */

	int model_count = (int)MODEL_ID_COUNT;
	Model* model_prefabs = arena_alloc(&model_data_arena, sizeof(*model_prefabs) * model_count);
	initialize_models(model_prefabs, model_count);

	StaticObjectArray so_array = test_initialize_static_objects(&scene_arena);

	while (!WindowShouldClose()) {
		arena_restore(&collider_data_arena, 0);

		if (IsKeyPressed(KEY_ESCAPE)) EnableCursor();
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P)) {
			if (loop_mode == GAMELOOP_GAME) {
				loop_mode = GAMELOOP_EDITOR;
				main_camera.projection = CAMERA_PERSPECTIVE;
			} else {
				loop_mode = GAMELOOP_GAME;
				main_camera.projection = CAMERA_PERSPECTIVE;
				main_camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
			}
		}
		if (loop_mode == GAMELOOP_EDITOR) {
			TriangleColliderArray static_colliders = static_object_loop(&collider_data_arena, so_array, model_prefabs);
			SpacialHash collider_spacial_hash = collision_spacial_hash_create(&collider_data_arena, static_colliders);
			editor_loop(
				&main_camera,
				so_array,
				model_prefabs,
				static_colliders,
				collider_spacial_hash
			);
		} else {
			main_game_loop(&main_camera);
		}
		
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();		// Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}