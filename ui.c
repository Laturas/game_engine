#ifndef AFTERHOURS
	#include "afterhours.h"
#endif

typedef enum UiElementMouseState {
	UIMS_NONE,
	UIMS_HOVERED,
	UIMS_CLICKED,
	UIMS_DOWN,
} UiElementMouseState;

bool is_mouse_hovered(Rectangle button_rect) {
	Vector2 mouse_position = GetMousePosition();
	return CheckCollisionPointRec(mouse_position, button_rect);
}

UiElementMouseState ui_element_mouse_state(Rectangle element_rect) {
	Vector2 mouse_position = GetMousePosition();
	if (CheckCollisionPointRec(mouse_position, element_rect)) {
		if (IsMouseButtonPressed(0)) {
			return UIMS_CLICKED;
		}
		if (IsMouseButtonDown(0)) {
			return UIMS_DOWN;
		}
		return UIMS_HOVERED;
	} else {
		return UIMS_NONE;
	}
}

void draw_editor_object_list(i32 x_pos, i32 y_pos) {
	char* objects[3] = {"Box", "Not box", "Sphere"};

	i32 obj_count = (sizeof(objects) / sizeof(*objects));
	i32 button_width = 300;
	i32 button_height = 20;

	i32 menu_height = 50					  /* Pre-button padding   */
		+ obj_count*((button_height - 1) + 5) /* Space for buttons	*/
		+ 0;								  /* After-button padding */

	DrawRectangle(x_pos, 10 + y_pos, 320, menu_height, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines(x_pos, 10 + y_pos, 320, menu_height, BLUE);

	DrawText("Objects", x_pos + 10, 20 + y_pos, 18, RAYWHITE);


	for (int i = 0; i < obj_count; i++) {
		i32 button_y_pos = y_pos + 50
			+ i*(button_height + 5); /* +5 for extra padding between buttons */
		i32 button_x_pos = x_pos + 10;

		Rectangle button = (Rectangle) {
			.height = button_height,
			.width = button_width,
			
			.x = button_x_pos,
			.y = button_y_pos
		};

		switch (ui_element_mouse_state(button)) {
			case UIMS_NONE: {
				DrawRectangle(	 button_x_pos, button_y_pos, 300, 20, Fade(SKYBLUE, 0.5f));

				DrawText(objects[i], button_x_pos + 5, button_y_pos + 5, 14, RAYWHITE);
			} break;

			case UIMS_HOVERED: {
				DrawRectangle(     button_x_pos, button_y_pos, 300, 20, Fade(BLUE, 0.5f));
				DrawRectangleLines(button_x_pos, button_y_pos, 300, 20, RAYWHITE);

				DrawText(objects[i], button_x_pos + 5, button_y_pos + 5,  14, RAYWHITE);
			} break;

			case UIMS_DOWN: {
				DrawRectangle(	   button_x_pos, button_y_pos, 300, 20, Fade(DARKBLUE, 0.5f));
				DrawRectangleLines(button_x_pos, button_y_pos, 300, 20, RAYWHITE);

				DrawText(objects[i], button_x_pos + 5, button_y_pos + 5,  14, RAYWHITE);
			} break;

			case UIMS_CLICKED: {
				DrawRectangle(	   button_x_pos, button_y_pos, 300, 20, Fade(DARKBLUE, 0.5f));
				DrawRectangleLines(button_x_pos, button_y_pos, 300, 20, RAYWHITE);

				DrawText(objects[i], button_x_pos + 5, button_y_pos + 5,  14, RAYWHITE);
				fprintf(stderr, "Rect clicked!\n");
			} break;	

			default: {
				ASSERT(("Case not handled!", 0));
			} break;
		}
	}
}

void draw_editor_ui() {
	Arena ui_draw_call_arena = {0};

	i32 fps_y_pos = 15 + 15;
	DrawFPS(15, fps_y_pos - 15);

	draw_editor_object_list(10, fps_y_pos);

	arena_free(&ui_draw_call_arena);
}