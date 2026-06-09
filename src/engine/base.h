#ifndef BASE_H
#define BASE_H

#include <raylib.h>

#define DEFAULT_INPUT_STR "Input..."

#define P_EPSILON 0.001f
#define MAX_SIZE_LABEL 20

#define BACKGROUND (Color){ 0x13, 0x13, 0x13, 0xFF }
#define AXIS_MAJOR_COLOR RAYWHITE
#define AXIS_MINOR_COLOR (Color) { 0x30, 0x30, 0x30, 0xFF }
#define AXIS_SUBMINOR_COLOR (Color) { 0x18, 0x18, 0x18, 0xFF }

#define CAMERA_SPEED 1.0f
#define ZOOM_SPEED 0.1f
#define DEFAULT_ZOOM 1.0f
#define MAX_ZOOM 20.0f
#define DEFAULT_STEP 30.0f
#define TICK_LABEL_SIZE 5
#define TICK_GAP 5
#define SUBTICK_SIZE 0.2f
#define RESOLUTION 500.0f

#define MAX_INPUT_CHARS 256

#define x_to_screen(x, cartesian, offset_x) (((x)-(offset_x))*(cartesian.x_scale)) + (cartesian.center_screen_x)
#define y_to_screen(y, cartesian, offset_y) -(((y)-(offset_y))*(cartesian.y_scale)) + (cartesian.center_screen_y)

#define screen_to_x(screen_x, cartesian, offset) (offset.x) + ((screen_x - cartesian.center_screen_x) / cartesian.x_scale)
#define screen_to_y(screen_y, cartesian, offset) (offset.y) - ((screen_y - cartesian.center_screen_y) / cartesian.y_scale)

#define screen_to_cartesian(pos, cartesian, camera) (Vector2) { \
    (screen_to_x((pos.x), (cartesian), ((camera).offset))),                     \
    (screen_to_y((pos.y), (cartesian), ((camera).offset)))                      \
}

#define max(l, r) ((l) > (r) ? (l) : (r))
#define min(l, r) ((l) > (r) ? (r) : (l))

typedef struct {
    float zoom;
    float old_zoom;
    Vector2 offset;
} PCamera;

typedef struct {
    int screen_width;
    int screen_height;
    float center_screen_x;
    float center_screen_y;
    float x_scale;
    float y_scale;
    float max_x;
    float min_x;
    float max_y;
    float min_y;
} Cartesian;

typedef struct {
    bool subgrid;
    bool grid;
} Options;

typedef struct {
    Vector2 start_pos;
    Vector2 end_pos;
    float size;
    float dir;
    Color color;
    bool is_moving;
    Vector2 move_increment;
    Vector2 target_pos;
    float time_move;
    char label[MAX_SIZE_LABEL+1];
} Vec2;

typedef struct {
    Vec2 *items;
    size_t count;
    size_t capacity;
} Vectors;

typedef struct {
    bool is_running;
    float time;
    float time_speed;
    bool subgrid;
    bool grid;
    bool is_inputting;
    PCamera camera;
    Vectors *v_ar;
    Cartesian cart;
    bool is_mouse_on_input;
    size_t input_count;
    char input_chars[MAX_INPUT_CHARS+1];
    const char *default_input_str;
    bool should_close;
} Engine;

void print_vector2(Vector2 v);
Vector2 cartesian_to_screen(Vector2 pos, Engine *engine);
bool is_near(float f0, float f1);


#endif // BASE_H
