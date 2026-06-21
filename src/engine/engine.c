// https://gamemath.com/book/
// https://natureofcode.com/
// https://mathfor3dgameprogramming.com/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <raylib.h>
#include <raymath.h>

#include "engine.h"
#include "parser.h"

static void user_interface_input(Engine *engine)
{
    if (!engine->is_inputting) {
        if (IsKeyPressed(KEY_S)) {
            engine->subgrid = !engine->subgrid;
        }
        if (IsKeyPressed(KEY_G)) {
            engine->grid = !engine->grid;
        }
        if (IsKeyDown(KEY_EQUAL)) {
            if (engine->camera.zoom < MAX_ZOOM) engine->camera.zoom += ZOOM_SPEED;
        }
        if (IsKeyDown(KEY_MINUS)) {
            if (engine->camera.zoom > 1.0f) engine->camera.zoom -= ZOOM_SPEED;
        }
        if (IsKeyDown(KEY_H)) {
            engine->camera.offset.x -= CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_L)) {
            engine->camera.offset.x += CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_K)) {
            engine->camera.offset.y += CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_J)) {
            engine->camera.offset.y -= CAMERA_SPEED;
        }
        if (IsKeyPressed(KEY_HOME)) {
            engine->camera.offset = (Vector2){0};
            engine->camera.zoom = DEFAULT_ZOOM;
            engine->time = 0.0f;
            engine->time_speed = 1.0f;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            engine->is_running = !engine->is_running;
        }
        if (IsKeyPressed(KEY_R)) {
            engine->time = 0.0f;
            engine->is_running = false;
            for (size_t i = 0; i < engine->v_obj->count; ++i) {
                Object *obj = &(engine->v_obj->items[i]);
                obj->pos = obj->original_pos;
                obj->speed = obj->original_speed;
                obj->velocity = obj->original_velocity;
            }
        }
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_PERIOD)) {
            engine->time_speed *= 2.0f;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_COMMA)) {
            engine->time_speed /= 2.0f;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (engine->obj_dragging != NULL) {
            Vector2 mouse_pos = GetMousePosition();
            mouse_pos = screen_to_cartesian(mouse_pos, engine->cart, engine->camera);
            Object *obj = engine->obj_dragging;
            obj->velocity = Vector2Scale(Vector2Subtract(mouse_pos, obj->pos), 1.0f/1.3f);
            print_vector2(obj->velocity);
            engine->obj_dragging = NULL;
            engine->is_running = true;
        }
    }
    if (engine->obj_dragging == NULL && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        for (size_t i = 0; i < engine->v_obj->count; ++i) {
            Object *obj = &(engine->v_obj->items[i]);
            const float pad = 15.0f;
            Vector2 pos = cartesian_to_screen(obj->pos, engine);
            if (CheckCollisionPointCircle(GetMousePosition(), pos,  obj->size+pad*2)) {
                engine->obj_dragging = obj;
                break;
            }
        }
        if (engine->obj_dragging == NULL) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, 0.030f/engine->camera.zoom);
            delta.x = -delta.x;
            engine->camera.offset = Vector2Add(engine->camera.offset, delta);
        }
    }
    if (engine->camera.zoom < 1.0f) engine->camera.zoom = 1.0f;

    float mouse_wheel;
    if ((mouse_wheel = GetMouseWheelMove()) != 0.0f) {
        Vector2 mouse_pos_before = screen_to_cartesian(GetMousePosition(), engine->cart, engine->camera);
        if (mouse_wheel > 0.0f && engine->camera.zoom < MAX_ZOOM) {
            engine->camera.zoom += (ZOOM_SPEED*5);
        } else if (mouse_wheel < 0.0f && engine->camera.zoom > 1.0f) {
            engine->camera.zoom -= (ZOOM_SPEED*5);
        }
        engine->cart.x_scale = engine->camera.zoom*DEFAULT_STEP;
        engine->cart.y_scale = engine->camera.zoom*DEFAULT_STEP;
        Vector2 mouse_pos_after = screen_to_cartesian(GetMousePosition(), engine->cart, engine->camera);

        Vector2 diff = Vector2Subtract(mouse_pos_before, mouse_pos_after);
        engine->camera.offset = Vector2Add(engine->camera.offset, diff);
    }

    if (engine->is_mouse_on_input) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            engine->is_inputting = true;
        }
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            engine->is_inputting = false;
            if (engine->input_count == 0) {
                strcpy(engine->input_chars, DEFAULT_INPUT_STR);
            }
        }
    }

    if (engine->is_inputting) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();
        while (key > 0) {
            if (engine->input_count < MAX_INPUT_CHARS) {
                engine->input_chars[engine->input_count] = (char)key;
                engine->input_chars[engine->input_count+1] = '\0';
                engine->input_count++;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (engine->input_count > 0) {
                engine->input_chars[engine->input_count-1] = '\0';
                engine->input_count--;
            }
        }
        if (IsKeyPressed(KEY_ENTER)) {
            parse_input(engine);
            engine->input_count = 0;
            engine->input_chars[engine->input_count] = '\0';
        }
    }
}


static void draw_grid(Engine engine)
{
    // x-axis and y-axis
    int pos_y = y_to_screen(0, engine.cart, engine.camera.offset.y);
    int pos_x = x_to_screen(0, engine.cart, engine.camera.offset.x);

    if (engine.subgrid) {
        for (int x = engine.cart.min_x; x <= engine.cart.max_x; ++x) {
            int pos_x = x_to_screen(x, engine.cart, engine.camera.offset.x);
            for (float sub_x = x+SUBTICK_SIZE; sub_x < x+1.0f; sub_x += SUBTICK_SIZE) {
                int pos_x = x_to_screen(sub_x, engine.cart, engine.camera.offset.x);
                DrawLine(pos_x, 0, pos_x, engine.cart.screen_height, AXIS_SUBMINOR_COLOR);
            }
            DrawLine(pos_x, 0, pos_x, engine.cart.screen_height, AXIS_MINOR_COLOR);
        }
        for (int y = engine.cart.min_y; y <= engine.cart.max_y; y += 1.0f) {
            int pos_y = y_to_screen(y, engine.cart, engine.camera.offset.y);
            for (float sub_y = y+SUBTICK_SIZE; sub_y < y+1.0f; sub_y += SUBTICK_SIZE) {
                int pos_y = y_to_screen(sub_y, engine.cart, engine.camera.offset.y);
                DrawLine(0, pos_y, engine.cart.screen_height, pos_y, AXIS_SUBMINOR_COLOR);
            }
            DrawLine(0, pos_y, engine.cart.screen_width, pos_y, AXIS_MINOR_COLOR);
        }
    }

    if (engine.grid) {
        DrawLine(0, pos_y, engine.cart.screen_width, pos_y, AXIS_MAJOR_COLOR);
        DrawLine(pos_x, 0, pos_x, engine.cart.screen_height, AXIS_MAJOR_COLOR);

        for (int x = engine.cart.min_x; x <= engine.cart.max_x; ++x) {
            int pos_x = x_to_screen(x, engine.cart, engine.camera.offset.x);
            if (engine.grid)
                DrawText(TextFormat("%d", (int)x), pos_x+TICK_GAP, pos_y+TICK_GAP, TICK_LABEL_SIZE, AXIS_MAJOR_COLOR);
        }

        for (int y = engine.cart.min_y; y <= engine.cart.max_y; y += 1.0f) {
            int pos_y = y_to_screen(y, engine.cart, engine.camera.offset.y);
            if (engine.grid)
                DrawText(TextFormat("%d", y), pos_x+TICK_GAP, pos_y+TICK_GAP, TICK_LABEL_SIZE, AXIS_MAJOR_COLOR);
        }
    }

}


void app(void (*simulation)(Engine*))
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1000, 1000, "p");
    SetTargetFPS(60);

    Vectors v_ar = {0};
    Object_Ar v_obj = {0};

    Engine engine = (Engine) {
        .is_running = false,
        .time = 0.0f,
        .time_speed = 1.0f,
        .subgrid = true,
        .grid = true,
        .is_inputting = false,
        .is_mouse_on_input = false,
        .input_count = 0,
        .v_ar = &v_ar,
        .v_obj = &v_obj,
        .should_close = false,
        .gravity_on = true,

    };
    strcpy(engine.input_chars, DEFAULT_INPUT_STR);

    engine.camera.old_zoom = DEFAULT_ZOOM;
    engine.camera.zoom = DEFAULT_ZOOM;

    simulation(&engine);

    for (size_t i = 0; i < v_obj.count; ++i) {
        Object *obj            = &(v_obj.items[i]);
        obj->original_pos      = obj->pos;
        obj->original_speed    = obj->speed;
        obj->original_velocity = obj->velocity;
    }

    while (!engine.should_close) {
        engine.should_close = WindowShouldClose();
        if (!engine.is_inputting && IsKeyPressed(KEY_Q)) {
            break;
        }
        user_interface_input(&engine);

        engine.cart.screen_width  = GetScreenWidth();
        engine.cart.screen_height = GetScreenHeight();
        engine.cart.center_screen_x = engine.cart.screen_width/2.0f;
        engine.cart.center_screen_y = engine.cart.screen_height/2.0f;
        const float raw_max_x = engine.cart.screen_width/DEFAULT_STEP;
        const float raw_max_y = engine.cart.screen_height/DEFAULT_STEP;
        engine.cart.x_scale = engine.camera.zoom*DEFAULT_STEP;
        engine.cart.y_scale = engine.camera.zoom*DEFAULT_STEP;
        engine.cart.max_x = engine.camera.offset.x+raw_max_x;
        engine.cart.min_x = engine.camera.offset.x-raw_max_x;
        engine.cart.max_y = engine.camera.offset.y+raw_max_y;
        engine.cart.min_y = engine.camera.offset.y-raw_max_y;

        Rectangle input_box = { 10, engine.cart.screen_height - 45, engine.cart.screen_width - 20, 40 };

        if (CheckCollisionPointRec(GetMousePosition(), input_box)) engine.is_mouse_on_input = true;
        else engine.is_mouse_on_input = false;


        if (engine.is_running) {
            float frame_time = GetFrameTime() * engine.time_speed;

            engine.time += frame_time;
            /* v_ar.items[0].dir += 0.01f * engine.time_speed; */
            /* v_ar.items[0].end_pos = vec2_endpoints(v_ar.items[0]); */
            // v1.dir += 0.05f;
            /* v_ar.items[2] = vec_add(v_ar.items[0], v_ar.items[1], RAYWHITE); */
            /* for (size_t i = 0; i < v_ar.count; ++i) { */
            /*     if (v_ar.items[i].time_move <= 0.0f) { */
            /*         v_ar.items[i].is_moving = false; */
            /*     } */
            /*     if (v_ar.items[i].is_moving) { */
            /*         float frame_time = GetFrameTime(); */
            /*         v_ar.items[i].time_move -= frame_time; */
            /*         v_ar.items[i].start_pos = Vector2Add(v_ar.items[i].start_pos, Vector2Scale(v_ar.items[i].move_increment, frame_time)); */
            /*     } */
            /* } */
            for (size_t i = 0; i < v_obj.count; ++i) {
                Object *obj = &(v_obj.items[i]);
                if (engine.gravity_on) {
                    obj->velocity.y += (frame_time * -GRAVITY_CONSTANT);
                }
                obj->pos.y += (obj->velocity.y*frame_time);
                obj->pos.x += (obj->velocity.x*frame_time);
                if (obj->pos.y <= 0) engine.is_running = false;
            }
        }

        BeginDrawing();

            ClearBackground(BACKGROUND);
            draw_grid(engine);


            if (0)
            { // vectors
                for (size_t i = 0; i < v_ar.count; ++i) {
                    draw_vector(v_ar.items[i], &engine); 
                }
            }

            if (1)
            { // objects
                for (size_t i = 0; i < v_obj.count; ++i) {
                    Object obj = v_obj.items[i];
                    Vector2 pos = cartesian_to_screen(obj.pos, &engine);
                    switch (obj.kind) {
                        case CIRCLE: {
                            DrawCircleV(pos, obj.size, obj.color);
                        } break;
                        case SQUARE: assert(false && "Not implemented");
                    }
                }
            }

            if (0)
            { // functions
                const float inc = 1.5f * ((engine.cart.max_x-engine.cart.min_x)/(RESOLUTION*engine.camera.zoom));

                for (float x = engine.cart.min_x; x <= engine.cart.max_x; x += inc) {
                    float y = sinf(x-engine.time*3);
                    Vector2 pos = cartesian_to_screen((Vector2) { x, y }, &engine);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, RED);

                    y = x*x*x;
                    pos = cartesian_to_screen((Vector2) { x, y }, &engine);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, SKYBLUE);

                    y = -x;
                    pos = cartesian_to_screen((Vector2) { x, y }, &engine);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, LIME);
                }
            }

            /* Vector2 mouse_pos = GetMousePosition(); */
            /* Vector2 pos = screen_to_cartesian(mouse_pos, engine.cart, engine.camera); */
            /* DrawText(TextFormat("(%.1f, %.1f)", pos.x, pos.y), mouse_pos.x, mouse_pos.y-30, 18, RAYWHITE); */

            for (size_t i = 0; i < v_ar.count; ++i) {
                Vec2 v = v_ar.items[i];
                Vector2 start_pos = cartesian_to_screen(v.start_pos, &engine);
                Vector2 end_pos = cartesian_to_screen(v.end_pos, &engine);
                int threshold = 30*engine.camera.zoom;
                float pad = 0.0f;
                Rectangle v_rec = (Rectangle) { 
                    min(start_pos.x, end_pos.x) - pad,
                    min(start_pos.y, end_pos.y) - pad,
                    fabsf(end_pos.x - start_pos.x) + pad*2,
                    fabsf(end_pos.y - start_pos.y) + pad*2 };
                if (CheckCollisionPointLine(GetMousePosition(), start_pos, end_pos, threshold)) {
                    DrawRectangleLinesEx(v_rec, 1.0f, v.color);
                    break;
                }
            }
            if (engine.obj_dragging != NULL) {
                const Object obj = *(engine.obj_dragging);
                const float pad = 15.0f;
                Vector2 pos = cartesian_to_screen(obj.pos, &engine);
                Rectangle v_rec = (Rectangle) { 
                    pos.x - pad*2,
                    pos.y - pad*2,
                    pad*4,
                    pad*4
                };
                DrawRectangleLinesEx(v_rec, 1.0f, obj.color);
                Vector2 mouse_pos = GetMousePosition();
                DrawLineEx(pos, mouse_pos, 2, obj.color);
            }

            // DrawFPS(5, 5);
            DrawText(TextFormat("zoom = %.1f", engine.camera.zoom), engine.cart.screen_width-85, 5, 16, RAYWHITE);

            DrawText(TextFormat("time = %.2fs", engine.time), 10, 5, 16, RAYWHITE);
            DrawText(TextFormat("time speed = %.2fx", engine.time_speed), 200, 5, 16, RAYWHITE);

            DrawRectangleRec(input_box, RAYWHITE);
            Color input_text_color = GRAY;
            if (engine.is_mouse_on_input) input_text_color = BLACK;
            DrawText(engine.input_chars, (int)input_box.x+10, (int)input_box.y+input_box.height/2.0f-10, 20, input_text_color);

        EndDrawing();
    }

    CloseWindow();
    // TODO free v_da
    /* da_free(&(engine.v_ar)); */

}
