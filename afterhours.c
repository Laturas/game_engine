#pragma once

#include "common.c"
#include "libs/include/raylib.h"
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

void draw_model(ModelID model_id, const Model* const model_prefabs, Transform model_transform) {
	if (model_id == MODEL_NONE) { return; }

	DrawModelEx(model_prefabs[model_id], model_transform.translation, VECTOR3_UP, 0.0f, model_transform.scale, WHITE);
}

void editor_loop(Camera* main_camera, StaticObjectArray static_objects, const Model* const model_prefabs, TriangleColliderArray optional_render_colliders) {
	update_editor_camera(main_camera);
	BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(*main_camera);
			// for (int i = 0; i < static_objects.len; i++) {
			// 	StaticObject current_object = static_objects.objects[i];
			// 	draw_model(current_object.id, model_prefabs, current_object.transform);
			// }

			for (int i = 0; i < optional_render_colliders.length; i++) {
				TriangleCollider tri = optional_render_colliders.colliders[i];
				DrawLine3D(tri.vert_1, tri.vert_2, LIME);
				DrawLine3D(tri.vert_2, tri.vert_3, LIME);
				DrawLine3D(tri.vert_3, tri.vert_1, LIME);
			}

			DrawGrid(10, 1.0f);
		EndMode3D();

		draw_editor_ui();

	EndDrawing();
}

void update_game_camera(Camera* camera) {
	Vector2 mousePositionDelta = GetMouseDelta();

	bool moveInWorldPlane = true;
	bool rotateAroundTarget = true;
	bool lockView = true;
	bool rotateUp = false;

	// Camera speeds based on frame time
	float cameraMoveSpeed = CAMERA_MOVE_SPEED*GetFrameTime();
	float cameraRotationSpeed = CAMERA_ROTATION_SPEED*GetFrameTime();
	float cameraPanSpeed = CAMERA_PAN_SPEED*GetFrameTime();
	float cameraOrbitalSpeed = CAMERA_ORBITAL_SPEED*GetFrameTime();

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

	object_array[2].transform.translation = (Vector3) {-3.0f,-3.0f,-3.0f,};

	return (StaticObjectArray) {
		.objects = object_array,
		.len = object_count
	};
}

int afterhours_main(int argc, char* argv[]) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	InitWindow(screenWidth, screenHeight, "Afterengine");
	SetTargetFPS(1000);
	SetExitKey(0); /* Disables ESC = exit */


	Camera3D main_camera = { 0 };
	main_camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
	main_camera.target = VECTOR3_ZERO;
	main_camera.up = VECTOR3_UP;
	main_camera.fovy = 45.0f;
	main_camera.projection = CAMERA_PERSPECTIVE;

	loop_mode = GAMELOOP_EDITOR;

	Arena model_data_arena = {0};
	int model_count = MODEL_ID_COUNT;
	Model* model_prefabs = arena_alloc(&model_data_arena, sizeof(*model_prefabs) * model_count);
	initialize_models(model_prefabs, model_count);


	Arena scratch_arena = {0};

	StaticObjectArray so_array = test_initialize_static_objects(&scratch_arena);

	// Main game loop
	Arena collider_data = {0};
	while (!WindowShouldClose()) {
		arena_restore(&collider_data, 0);

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
			TriangleColliderArray colliders = static_object_loop(&collider_data, so_array, model_prefabs);
			editor_loop(
				&main_camera,
				so_array,
				model_prefabs,
				colliders
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