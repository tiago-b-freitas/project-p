#include <stdio.h>
#include <raylib.h>

#include <math.h>

#define BACKGROUND (Color){ 0x13, 0x13, 0x13, 0xFF }
#define CAMERA_SPEED 1.1f
#define ZOOM_SPEED 0.1f
#define DEFAULT_ZOOM 3.0f
#define MIN_X -10.0f*(DEFAULT_ZOOM)
#define MAX_X -(MIN_X)
#define MIN_Y -10.0f*(DEFAULT_ZOOM)
#define MAX_Y -(MIN_Y)
#define DEFAULT_STEP 30.0f
#define TICK_STEP 1.0f
#define TICK_SIZE 5.0f
#define RESOLUTION 500.0f

typedef struct {
    float zoom;
    Vector2 offset;
} PCamera;

#define cartesian_to_screen(x, y, cartesian, offset_x, offset_y) (Vector2){ \
 ((x)-(offset_x))*(cartesian.x_scale) + (cartesian.center_screen_x), \
 -(((y)-(offset_y))*(cartesian.y_scale)) + (cartesian.center_screen_y)  \
}

typedef struct {
    int center_screen_x;
    int center_screen_y;
    float x_scale;
    float y_scale;
} Cartesian;

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 800, "p");
    SetTargetFPS(30);

    PCamera camera = {0};
    camera.zoom = DEFAULT_ZOOM;

    while (!WindowShouldClose()) {
        const int screen_width  = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const Cartesian cartesian = {
            .center_screen_x = screen_width/2,
            .center_screen_y = screen_height/2,
            .x_scale = camera.zoom*DEFAULT_STEP,
            .y_scale = camera.zoom*DEFAULT_STEP
        };
        if (IsKeyPressed(KEY_Q)) {
            break;
        }
        if (IsKeyDown(KEY_EQUAL)) {
            camera.zoom += ZOOM_SPEED;
        }
        if (IsKeyDown(KEY_MINUS)) {
            if (camera.zoom > 1.0f) camera.zoom -= ZOOM_SPEED;
        }
        if (IsKeyDown(KEY_H)) {
            camera.offset.x += CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_L)) {
            camera.offset.x -= CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_K)) {
            camera.offset.y += CAMERA_SPEED;
        }
        if (IsKeyDown(KEY_J)) {
            camera.offset.y -= CAMERA_SPEED;
        }
        if (IsKeyPressed(KEY_HOME)) {
            camera.offset = (Vector2){0};
            camera.zoom = DEFAULT_ZOOM;
        }
        if (camera.zoom < 1.0f) camera.zoom = 1.0f;


        BeginDrawing();

            ClearBackground(BACKGROUND);

            
            float max_x = (screen_width/DEFAULT_STEP)+camera.offset.x;
            float min_x = -(screen_width/DEFAULT_STEP)+camera.offset.x;

            float max_y = (screen_height/DEFAULT_STEP)+camera.offset.y;
            float min_y = -(screen_height/DEFAULT_STEP)+camera.offset.y;

            {
                Vector2 start_pos = cartesian_to_screen(min_x, 0, cartesian, camera.offset.x, camera.offset.y);
                Vector2 end_pos   = cartesian_to_screen(max_x, 0, cartesian, camera.offset.x, camera.offset.y);
                DrawLineV(start_pos, end_pos, RAYWHITE);
                for (int x = min_x; x <= max_x; x += 1.0f) {
                    Vector2 pos = cartesian_to_screen(x, 0, cartesian, camera.offset.x, camera.offset.y);
                    DrawText(TextFormat("%d", x), (int)pos.x, (int)pos.y, 5, RAYWHITE);
                }
            }
            {
                Vector2 start_pos = cartesian_to_screen(0, min_y, cartesian, camera.offset.x, camera.offset.y);
                Vector2 end_pos   = cartesian_to_screen(0, max_y, cartesian, camera.offset.x, camera.offset.y);
                DrawLineV(start_pos, end_pos, RAYWHITE);
                for (int y = min_y; y <= max_y; y += 1.0f) {
                    Vector2 pos = cartesian_to_screen(0, y, cartesian, camera.offset.x, camera.offset.y);
                    DrawText(TextFormat("%d", y), (int)pos.x, (int)pos.y, 5, RAYWHITE);
                }
            }
            {
                const float inc = (float)((max_x-min_x)/(RESOLUTION*sqrtf(camera.zoom)));

                for (float x = min_x; x <= max_x; x += inc) {
                    float y = sinf(x);
                    Vector2 pos = cartesian_to_screen(x, y, cartesian, camera.offset.x, camera.offset.y);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, RED);

                    y = x*x*x;
                    pos = cartesian_to_screen(x, y, cartesian, camera.offset.x, camera.offset.y);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, SKYBLUE);

                    y = -x;
                    pos = cartesian_to_screen(x, y, cartesian, camera.offset.x, camera.offset.y);
                    DrawRectangleV(pos, (Vector2) { 4.0f, 4.0f }, LIME);
                }
            }

            DrawFPS(5, 5);
            DrawText(TextFormat("zoom = %.1f", camera.zoom), GetScreenWidth()-85, 5, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// TODO
// 1. ajuster a velocidade de zoom e de movimento de acordo com o nível de zoom. Quando maior o zoom maior a velocidade movimento e maior a aproximação para mais-zoom.
