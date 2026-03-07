/**
* Handles the immediate mode UI for the engine editor interface.
*
* It isn't very good right now. Heavily WIP. 
*/
#include "libs/include/raylib.h"
#ifndef AFTERHOURS_H
	#include "common.c"
#endif

/*
Example usage code:

UIList list = imui_start_list(&ui_arena, &ui_context);

	for (int i = 0; i < some_number; i++) {
		imui_draw_text(&ui_arena, &ui_context, "Hello!");
	}

imui_end_list(&ui_arena, &ui_context, &list); <- Parses through and determines the size.

imui_render_context(ui_context);
*/

#ifndef REGION_COMMANDS

typedef enum UICommandType {
	IMUI_NONE = 0,

	IMUI_FPS,
	IMUI_PAGE_LIST,
	IMUI_TEXT,
	IMUI_BUTTON,
	IMUI_CHECKBOX,
	IMUI_REGION,
	IMUI_END_REGION,
	IMUI_PADDING,

	IMUI_COUNT
} UICommandType;

typedef enum UIDirection {
	IMDIR_HORIZONTAL,
	IMDIR_VERTICAL
} UIDirection;

typedef struct UICommandNone {
	int none;
} UICommandNone;

typedef struct UICommandFPS {
	int none;
} UICommandFPS;

typedef struct UICommandPadding {
	int padding_amount;
} UICommandPadding;

typedef struct UICommandText {
	String text;
	Color color;
	float text_fade;
	float font_size;
} UICommandText;

typedef struct UICommandButton {
	String text;
	float internal_padding;
	float font_size;
	Color background_color;
	Color background_hover_color;
	Color background_click_color;
	Vector2 override_size;

	void* click_callback;
} UICommandButton;

typedef struct UICommandPageList {
	int current_page;
} UICommandPageList;

typedef struct UIRegionParameters {
	int vertical_spacing;
	int horizontal_spacing;

	Color background_color;
	f32 background_fade;

	Color border_color;
	f32 border_fade;
} UIRegionParameters;

typedef struct UICommandRegion {
	Rectangle bounding_rect;
	UIDirection direction;
	UIRegionParameters params;
} UICommandRegion;

typedef struct UICommandEndRegion {
	int none;
} UICommandEndRegion;

#endif

typedef union UIUnion {
	UICommandNone none;
	UICommandRegion region;
	UICommandEndRegion end_region;
	UICommandFPS fps;
	UICommandText text;
	UICommandButton button;
	UICommandPadding padding;
} UIUnion;

typedef struct UICommand {
	void* callback_fptr;
	struct UICommand* next_command;
	UICommandType type;
	UIUnion command_data;
} UICommand;

typedef struct UICommandContext {
	UICommand* head;
	UICommand* tail;
} UICommandContext;

#ifndef REGION_EXTRA

typedef enum UIMouseState {
	UIMS_NONE,
	UIMS_HOVERED,
	UIMS_CLICKED,
	UIMS_DOWN
} UIMouseState;

UIMouseState imui_element_mouse_state(Rectangle element_rect) {
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

#endif

void imui_draw_fps(Arena* ui_arena, UICommandContext* ui_context) {
	UICommand* command = arena_alloc(ui_arena, sizeof(*command));

	command->command_data.fps = (UICommandFPS) {
		0
	};
	command->type = IMUI_FPS;
	command->callback_fptr = NULL;

	if (ui_context->head == NULL) {
		ui_context->head = command;
		ui_context->tail = command;
	} else {
		ui_context->tail->next_command = command;
		ui_context->tail = command;
	}
}

void imui_draw_padding(Arena* ui_arena, UICommandContext* ui_context, int padding_amount) {
	UICommand* command = arena_alloc(ui_arena, sizeof(*command));

	command->command_data.padding = (UICommandPadding) {
		.padding_amount = padding_amount
	};
	command->type = IMUI_PADDING;
	command->callback_fptr = NULL;

	if (ui_context->head == NULL) {
		ui_context->head = command;
		ui_context->tail = command;
	} else {
		ui_context->tail->next_command = command;
		ui_context->tail = command;
	}
}

void imui_draw_text(Arena* ui_arena, UICommandContext* context,
	String text,
	Color color,
	float text_fade,
	float font_size
) {
	UICommand* text_command = arena_alloc(ui_arena, sizeof(*text_command));

	text_command->next_command = NULL;
	text_command->callback_fptr = NULL;
	text_command->type = IMUI_TEXT;
	text_command->command_data.text = (UICommandText) {
		.color = color,
		.font_size = font_size,
		.text = text,
		.text_fade = text_fade
	};

	if (context->head == NULL) {
		context->head = text_command;
		context->tail = text_command;
	} else {
		context->tail->next_command = text_command;
		context->tail = text_command;
	}
}

void imui_draw_button(Arena* ui_arena, UICommandContext* context,
	String text,
	Color background_color,
	Color background_hover_color,
	Color background_click_color,
	float font_size,
	float internal_padding,
	Vector2 optional_override_size
) {
	UICommand* button_command = arena_alloc(ui_arena, sizeof(*button_command));

	button_command->next_command = NULL;
	button_command->callback_fptr = NULL;
	button_command->type = IMUI_BUTTON;
	button_command->command_data.button = (UICommandButton) {
		.background_color = background_color,
		.background_click_color = background_click_color,
		.background_hover_color = background_hover_color,
		.click_callback = NULL,
		.font_size = font_size,
		.internal_padding = internal_padding,
		.text = text,
		.override_size = optional_override_size
	};

	if (context->head == NULL) {
		context->head = button_command;
		context->tail = button_command;
	} else {
		context->tail->next_command = button_command;
		context->tail = button_command;
	}
}

void imui_region_begin(
	Arena* ui_arena,
	UICommandContext* context,
	Rectangle region_rect,
	UIDirection direction,
	UIRegionParameters region_params
) {
	UICommand* new_region = arena_alloc(ui_arena, sizeof(*new_region));

	new_region->next_command = NULL;
	new_region->callback_fptr = NULL;
	new_region->type = IMUI_REGION;
	new_region->command_data.region = (UICommandRegion) {
		.bounding_rect = region_rect,
		.direction = direction,
		.params = region_params
	};

	if (context->head == NULL) {
		context->head = new_region;
		context->tail = new_region;
	} else {
		context->tail->next_command = new_region;
		context->tail = new_region;
	}
}

void imui_region_end(Arena* ui_arena, UICommandContext* context) {
	UICommand* end_region = arena_alloc(ui_arena, sizeof(*end_region));

	end_region->next_command = NULL;
	end_region->callback_fptr = NULL;
	end_region->type = IMUI_END_REGION;

	if (context->head == NULL) {
		context->head = end_region;
		context->tail = end_region;
	} else {
		context->tail->next_command = end_region;
		context->tail = end_region;
	}
}

UICommand* imui_render_region_internal(UICommandContext context) {
	UICommand* command = context.head;
	Rectangle rect = command->command_data.region.bounding_rect;
	UIDirection dir = command->command_data.region.direction;
	float bg_fade = command->command_data.region.params.background_fade; 
	float border_fade = command->command_data.region.params.border_fade;

	Vector2 cursor_position = (Vector2) { rect.x + command->command_data.region.params.horizontal_spacing, rect.y + command->command_data.region.params.vertical_spacing }; 

	if (bg_fade > 0.0f) {
		Color rect_color = Fade(command->command_data.region.params.background_color, command->command_data.region.params.background_fade);
		DrawRectangle(rect.x, rect.y, rect.width, rect.height, rect_color);
	}
	if (border_fade > 0.0f) {
		Color border_color = Fade(command->command_data.region.params.border_color, command->command_data.region.params.border_fade);
		DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, border_color);
	}

	command = command->next_command;
	while (command != NULL) {
		int update_by = 0;
		switch (command->type) {
			case IMUI_NONE: break;

			case IMUI_TEXT: {
				UICommandText text_data = command->command_data.text;
				Color text_color = Fade(text_data.color, text_data.text_fade);
				DrawTextEx(GetFontDefault(), text_data.text.str, cursor_position, text_data.font_size, 2.0f, text_color);

				update_by = (dir == IMDIR_VERTICAL) ? (int)text_data.font_size : text_data.text.length * (int)text_data.font_size;
			} break;

			case IMUI_FPS: {
				DrawFPS(cursor_position.x, cursor_position.y);
				update_by = (dir == IMDIR_VERTICAL) ? 15 : 50;
			} break;

			case IMUI_REGION: {
				UICommandContext tmp_context = { .head = command, .tail = context.tail };
				UICommand* next = imui_render_region_internal(tmp_context);

				update_by = (dir == IMDIR_VERTICAL) ? command->command_data.region.bounding_rect.height : command->command_data.region.bounding_rect.width;
				command = next;
			} break;

			case IMUI_BUTTON: {
				UICommandButton text_data = command->command_data.button;
				
				Vector2 text_size = MeasureTextEx(GetFontDefault(), text_data.text.str, text_data.font_size, 1.0f);
				
				Rectangle button_region;
				button_region.width = text_size.x + (2 * text_data.internal_padding);
				button_region.height = text_size.y + (2 * text_data.internal_padding);
				button_region.x = cursor_position.x;
				button_region.y = cursor_position.y;

				if (text_data.override_size.x > 0.0f) { button_region.width = text_data.override_size.x; }
				if (text_data.override_size.y > 0.0f) { button_region.height = text_data.override_size.y; }

				Color fill_color;
				Color border_color;

				switch (imui_element_mouse_state(button_region)) {
					case UIMS_NONE: {
						fill_color = text_data.background_color;
						border_color = Fade(WHITE, 0.0f);
					} break;

					case UIMS_HOVERED: {
						fill_color = text_data.background_hover_color;
						border_color = Fade(WHITE, 1.0f);
					} break;

					case UIMS_CLICKED:
						fprintf(stderr, "Rect clicked!\n");
					/* fallthrough */
					case UIMS_DOWN: {
						fill_color = text_data.background_click_color;
						border_color = Fade(WHITE, 1.0f);
					} break;
				}

				DrawRectangle(	   button_region.x, button_region.y, button_region.width, button_region.height, fill_color);
				DrawRectangleLines(button_region.x, button_region.y, button_region.width, button_region.height, border_color);
				DrawText(text_data.text.str, button_region.x + text_data.internal_padding, button_region.y + text_data.internal_padding, (int)text_data.font_size, RAYWHITE);

				update_by = (dir == IMDIR_VERTICAL) ? button_region.height : button_region.width;
			} break;

			case IMUI_PADDING: {
				UICommandPadding padding_data = command->command_data.padding;
				update_by = padding_data.padding_amount;
			} break;

			case IMUI_END_REGION: {
				return command;
			} break;

			case IMUI_COUNT: {
				ASSERT("Illegal enumeration value");
			} break;
		}

		if (dir == IMDIR_VERTICAL) {
			cursor_position.y += update_by;
		} else {
			cursor_position.x += update_by;
		}

		command = command->next_command;
	}
	return NULL;
}

void imui_context_render(UICommandContext context) {
	if (context.head == NULL) { return; }

	if (context.head->type != IMUI_REGION) {
		return;
	}

	imui_render_region_internal(context);
}