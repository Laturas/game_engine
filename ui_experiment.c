#include "libs/include/raylib.h"
#ifndef AFTERHOURS_H
	#include "common.c"
	#include "hash_map.c"
#endif

typedef enum UiDirection {
	DIRECTION_HORIZONTAL,
	DIRECTION_VERTICAL
} UiDirection;

typedef enum UIFeatureFlags {
	UIFF_REGION     = (1 << 0),
	UIFF_TEXT       = (1 << 1),
	UIFF_PANEL      = (1 << 2),
	UIFF_BORDER     = (1 << 3),
	UIFF_BACKGROUND = (1 << 4),

	UIFF_COUNT,
} UIFeatureFlags;

typedef struct UIElement {
	UIFeatureFlags flags;

	/* Region params */
	Rectangle bounding_rect;
	UiDirection direction;
	Vector2 cursor;
	int margin_x;
	int margin_y;
	int element_padding_x;
	int element_padding_y;

	/* Fill params */
	Color background_color;

	/* Border params */
	Color border_color;

	/* Text params */
	Font font;
	char* text;
	int font_size;
	Color text_color;
} UIElement;

/* Stupid fatass */
typedef struct UiContext {
	Arena arena;

	UIElement* region_stack;
	UIElement* elements;
	int stack_count;
	int stack_capacity;

	int vertical_spacing;
	int horizontal_spacing;

	bool is_panel_size_fixed;
	int fixed_panel_width;
	int fixed_panel_height;
	Color panel_fill_color;
	Color panel_border_color;
	
	bool is_button_size_fixed;
	int fixed_button_width;
	int fixed_button_height;

	Font current_font;
	
	Vector2 global_cursor;
} UiContext;

typedef struct UiResponse {
	bool is_clicked;
	bool is_dragged;
	bool is_hovered;
} UiResponse;

void eui_fix_panel_size(UiContext* context, int fixed_panel_width, int fixed_panel_height) {
	if (context == NULL) { return; }
	context->is_panel_size_fixed = true;
	context->fixed_panel_height  = fixed_panel_height;
	context->fixed_panel_width   = fixed_panel_width;
}

void eui_unfix_panel_size(UiContext* context) {
	if (context == NULL) { return; }
	context->is_panel_size_fixed = false;
}

/**
 * eui_fix_button_size(&context, 50, 5);
 * 
 * // The below are now fixed size
 * eui_button(&ui_arena, &context, "Hello");
 * eui_button(&ui_arena, &context, "Button 2");
 * 
 * eui_unfix_button_size(&context);
 */
void eui_fix_button_size(UiContext* context, int fixed_button_width, int fixed_button_height) {
	if (context == NULL) { return; }
	context->is_button_size_fixed = true;
	context->fixed_button_height  = fixed_button_height;
	context->fixed_button_width   = fixed_button_width;
}

void eui_unfix_button_size(UiContext* context) {
	if (context == NULL) { return; }
	context->is_button_size_fixed = false;
}

UiResponse eui_vertical_panel_region_start(UiContext* context, int width, int height) {
	ASSERT(context->stack_capacity != context->stack_count);

	UIElement* new_region = &context->region_stack[context->stack_count];
	context->stack_count++;

	Vector2 new_cursor;

	if (context->stack_count > 0) {
		new_cursor = context->region_stack[context->stack_count - 1].cursor;

		new_cursor.x += context->region_stack->element_padding_x;
		new_cursor.y += context->region_stack->element_padding_y;
	} else {
		new_cursor = context->global_cursor;
	}

	Rectangle bounding_rect;

	if (context->is_panel_size_fixed) {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = context->fixed_panel_height,
			.width = context->fixed_panel_width
		};
	} else {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = 0,
			.width = 0
		};
	}

	*new_region = (UIElement) {
		.flags = UIFF_REGION,

		.bounding_rect = bounding_rect,
		.cursor = new_cursor,
		.direction = DIRECTION_VERTICAL
	};
}

UiResponse eui_horizontal_panel_region_start(UiContext* context, int width, int height) {

}

UiResponse eui_panel_region_end(UiContext* context) {

}

void eui_vertical_region_start(UiContext* context) {
	ASSERT(context->stack_capacity != context->stack_count);

	UIElement* new_region = &context->region_stack[context->stack_count];
	context->stack_count++;

	Vector2 new_cursor;

	if (context->stack_count > 0) {
		new_cursor = context->region_stack[context->stack_count - 1].cursor;

		new_cursor.x += context->region_stack->element_padding_x;
		new_cursor.y += context->region_stack->element_padding_y;
	} else {
		new_cursor = context->global_cursor;
	}

	Rectangle bounding_rect;

	if (context->is_panel_size_fixed) {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = context->fixed_panel_height,
			.width = context->fixed_panel_width
		};
	} else {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = 0,
			.width = 0
		};
	}

	*new_region = (UIElement) {
		.flags = UIFF_REGION,

		.bounding_rect = bounding_rect,
		.cursor = new_cursor,
		.direction = DIRECTION_VERTICAL
	};
}

void eui_horizontal_region_start(UiContext* context) {
	ASSERT(context->stack_capacity != context->stack_count);

	UIElement* new_region = &context->region_stack[context->stack_count];
	context->stack_count++;

	Vector2 new_cursor;

	if (context->stack_count > 0) {
		new_cursor = context->region_stack[context->stack_count - 1].cursor;

		new_cursor.x += context->region_stack->element_padding_x;
		new_cursor.y += context->region_stack->element_padding_y;
	} else {
		new_cursor = context->global_cursor;
	}

	Rectangle bounding_rect;

	if (context->is_panel_size_fixed) {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = context->fixed_panel_height,
			.width = context->fixed_panel_width
		};
	} else {
		bounding_rect = (Rectangle) {
			.x = new_cursor.x,
			.y = new_cursor.y,
			.height = 0,
			.width = 0
		};
	}

	*new_region = (UIElement) {
		.flags = UIFF_REGION,

		.bounding_rect = bounding_rect,
		.cursor = new_cursor,
		.direction = DIRECTION_HORIZONTAL
	};
}

void eui_region_end(UiContext* context) {
	ASSERT(context->stack_count != 0);

	UIElement* current_region = &context->region_stack[context->stack_count - 1];
	context->stack_count--;

	/* == 0 indicates dynamically sized */
	if (current_region->bounding_rect.height == 0) {
		current_region->bounding_rect.height = current_region->margin_y + current_region->cursor.y;
	}
	if (current_region->bounding_rect.width == 0) {
		current_region->bounding_rect.width = current_region->margin_x + current_region->cursor.x;
	}
	
	if (context->stack_count > 0) {
		UIElement* parent_region = &context->region_stack[context->stack_count - 1];

		ASSERT(parent_region->direction == DIRECTION_HORIZONTAL || parent_region->direction == DIRECTION_VERTICAL);

		if (parent_region->direction == DIRECTION_HORIZONTAL) {
			parent_region->cursor.x += current_region->bounding_rect.width;
		} else {
			parent_region->cursor.y += current_region->bounding_rect.height;
		}
	} else {
		context->global_cursor.y = current_region->bounding_rect.height + (float)context->vertical_spacing;
	}
}

UiResponse eui_padding(UiContext* context, int padding_amount) {

}

UiResponse eui_button(UiContext* context, const char* text) {

}

UiResponse eui_text(UiContext* context, const char* text) {

}

void eui_draw_context(UiContext* context) {
	/* All regions should be closed by this point */
	ASSERT(context->stack_count == 0);

	UIElement* current = context->elements;

	while (current != NULL) {
		if (current->flags & UIFF_BACKGROUND) {
			DrawRectangle(current->bounding_rect.x, current->bounding_rect.y, current->bounding_rect.width, current->bounding_rect.height, current->background_color);
		}
		if (current->flags & UIFF_BORDER) {
			DrawRectangleLines(current->bounding_rect.x, current->bounding_rect.y, current->bounding_rect.width, current->bounding_rect.height, current->border_color);
		}
		if (current->flags & UIFF_TEXT) {
			Vector2 pos = { .x = current->bounding_rect.x, .y = current->bounding_rect.y };
			DrawTextEx(current->font, current->text, pos, current->font_size, 0.0f, current->text_color);
		}
	}
}

#define UI_REGION_STACK_DEPTH 256

UiContext eui_context_create() {
	UiContext new_context = {0};

	new_context.region_stack = arena_alloc(&new_context.arena, sizeof(*new_context.region_stack) * UI_REGION_STACK_DEPTH);
	new_context.elements = NULL;
	new_context.stack_capacity = UI_REGION_STACK_DEPTH;
	new_context.stack_count = 0;

	new_context.horizontal_spacing = 5;
	new_context.vertical_spacing   = 5;

	new_context.is_panel_size_fixed  = false;
	new_context.is_button_size_fixed = false;

	new_context.current_font = GetFontDefault();

	return new_context;
}

void eui_context_destroy(UiContext* context) {
	if (context == NULL) { return; }

	arena_free(&context->arena);
}

void test_example() {
	UiContext ctx = eui_context_create();
		eui_vertical_region_start(&ctx);

			eui_horizontal_region_start(&ctx);
				eui_text(&ctx, "Hello button: ");
				eui_button(&ctx, "Hello!");
			eui_region_end(&ctx);

			eui_horizontal_region_start(&ctx);
				eui_text(&ctx, "More buttons: ");
				eui_button(&ctx, "Button 2");
			eui_region_end(&ctx);

			if (eui_button(&ctx, "Clickable!").is_clicked) {
				
			}

		eui_region_end(&ctx);

	eui_draw_context(&ctx);
	eui_context_destroy(&ctx);
}