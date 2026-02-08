#ifndef AFTERHOURS
    #include "afterhours.h"
#endif

Rectangle draw_editor_object_list_button(char* text, i32 x_pos, i32 y_pos) {
    DrawRectangle( x_pos, y_pos, 300, 20, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines( x_pos, y_pos, 300, 20, BLUE);
    DrawText(text, x_pos + 5, y_pos + 5, 14, RAYWHITE);

    return (Rectangle) {
        .height = 20,
        .width = 300,
        
        .x = x_pos,
        .y = y_pos
    };
}

bool is_clicked(Rectangle button_rect) {

}

void draw_editor_object_list(i32 x_pos, i32 y_pos) {
    DrawRectangle( x_pos, 10 + y_pos, 320, 93, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines( x_pos, 10 + y_pos, 320, 93, BLUE);

    DrawText("Objects", x_pos + 10, 20 + y_pos, 18, RAYWHITE);

    char* objects[2] = {"Box", "Not box"};

    for (int i = 0; i < (sizeof(objects) / sizeof(*objects)); i++) {
        i32 button_y_pos = y_pos + 40 + i*25;
        Rectangle button = draw_editor_object_list_button(objects[i], x_pos + 10, button_y_pos);
    }

    // DrawRectangle( x_pos + 10, 40 + y_pos, 300, 20, Fade(SKYBLUE, 0.5f));
    // DrawRectangleLines( x_pos + 10, 40 + y_pos, 300, 20, BLUE);
    // DrawText("Box", x_pos + 15, 45 + y_pos, 14, RAYWHITE);
}

typedef enum UiDrawType {
    DRAW_NULL = 0,
    DRAW_FPS,
    DRAW_BUTTON,
    DRAW_RECT,
} UiDrawType;

typedef struct DrawCommand {

} DrawCommand;

void draw_editor_ui() {
    Arena ui_draw_call_arena = {0};

    i32 fps_y_pos = 15 + 10;
    DrawFPS(15, fps_y_pos - 10);

    draw_editor_object_list(10, fps_y_pos);

    arena_free(&ui_draw_call_arena);
}