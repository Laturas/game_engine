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
	UICommandFPS fps;
	UICommandRegion region;
	UICommandEndRegion end_region;
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

void imui_render_region(UICommandContext context, UICommandRegion region) {
	UICommand* command = context.head;
	Rectangle rect = region.bounding_rect;
	float bg_fade = command->command_data.region.params.background_fade; 
	float border_fade = command->command_data.region.params.border_fade; 

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
		switch (command->type) {
			case IMUI_NONE: break;

			case IMUI_TEXT: {

			} break;
		}
	}
}

void imui_context_render(UICommandContext context) {
	
}