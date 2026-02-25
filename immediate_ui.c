/**
* Handles the immediate mode UI for the engine editor interface.
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

typedef struct UICommandText {
	String text;
	Color color;
	float text_fade;
	float font_size;
} UICommandText;

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

void imui_draw_text(Arena* ui_arena, UICommandContext* context, String text, Color color, float text_fade, float font_size) {
	UICommand* text_command = arena_alloc(ui_arena, sizeof(*text_command));

	text_command->next_command = NULL;
	text_command->callback_fptr = NULL;
	text_command->type = IMUI_REGION;
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

void imui_render_region(UICommandContext context) {
	UICommand* command = context.head;
	Rectangle rect = command->command_data.region.bounding_rect;
	UIDirection dir = command->command_data.region.direction;
	float bg_fade = command->command_data.region.params.background_fade; 
	float border_fade = command->command_data.region.params.border_fade;

	Vector2 cursor_position = (Vector2) { rect.x, rect.y }; 

	if (bg_fade > 0.0f) {
		Color rect_color = Fade(command->command_data.region.params.background_color, command->command_data.region.params.background_fade);
		DrawRectangle(rect.x, rect.y, rect.width, rect.height, rect_color);
	}
	if (border_fade > 0.0f) {
		Color border_color = Fade(command->command_data.region.params.border_color, command->command_data.region.params.border_fade);
		DrawRectangle(rect.x, rect.y, rect.width, rect.height, border_color);
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
			} break;

			case IMUI_REGION: {
				UICommandContext tmp_context = { .head = command, .tail = context.tail };
				imui_render_region(tmp_context);

				update_by = (dir == IMDIR_VERTICAL) ? command->command_data.region.bounding_rect.height : command->command_data.region.bounding_rect.width;
			} break;

			case IMUI_END_REGION: {
				return;
			} break;
		}

		if (dir == IMDIR_VERTICAL) {
			cursor_position.y += update_by;
		} else {
			cursor_position.x += update_by;
		}

		command = command->next_command;
	}
}

void imui_context_render(UICommandContext context) {
	
}