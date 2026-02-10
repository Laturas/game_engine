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

void editor_loop(Camera* main_camera) {
	update_editor_camera(main_camera);
	BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(*main_camera);
			// DrawCube((Vector3) {0,0,0}, 2.0f, 2.0f, 2.0f, DARKGREEN);
			// DrawCubeWires(VECTOR3_ZERO, 2.0f, 2.0f, 2.0f, MAROON);

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

int afterhours_main(int argc, char* argv[]) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	InitWindow(screenWidth, screenHeight, "Afterengine");
	SetTargetFPS(60);

	// Define the camera to look into our 3d world
	Camera3D main_camera = { 0 };
	main_camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
	main_camera.target = VECTOR3_ZERO;	  // Camera looking at point
	main_camera.up = VECTOR3_UP;		  // Camera up vector (rotation towards target)
	main_camera.fovy = 45.0f;
	main_camera.projection = CAMERA_PERSPECTIVE;

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
	SetExitKey(0); // Disables ESC = exit
	// DisableCursor();

	loop_mode = GAMELOOP_EDITOR;

	Arena scratch_arena = {0};

	// StaticObjectArray* so_array = arena_alloc(&scratch_arena, sizeof(*so_array));
	// StaticObject* static_test_cube = arena_alloc(&scratch_arena, sizeof(*static_test_cube));

	// static_test_cube->id = SOBJ_BOX;
	// static_test_cube->layer = MASK_STATIC_GEOMETRY;
	// static_test_cube->transform = default_transform();
	// static_test_cube->transform.translation = (Vector3) {2.0f, 0.0f, 0.0f};

	// StaticObjectArray so_array = {.objects = static_test_cube, .len = 1};

	// Main game loop
	Arena collider_data = {0};
	while (!WindowShouldClose()) {
		arena_restore(&collider_data, 0);
		// collider_loop(&collider_data,
		// 	(TriangleColliderArray){.colliders = NULL, .length = 0},
		// 	(TriangleColliderArray){.colliders = NULL, .length = 0}
		// );
		// static_object_loop(so_array, &collider_data);

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
			editor_loop(&main_camera);
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