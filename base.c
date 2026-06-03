#include "base.h"

void print_vector2(Vector2 v)
{
    printf("x = %.1f; y = %.1f\n", v.x, v.y);
}

Vector2 cartesian_to_screen(Vector2 pos, Cartesian cart, PCamera camera)
{
    return (Vector2) {
        .x =  ((pos.x - camera.offset.x)*cart.x_scale) + cart.center_screen_x,
        .y = -((pos.y - camera.offset.y)*cart.y_scale) + cart.center_screen_y,
    };
}


void print_vec(Vec2 v)
{
    printf("start_pos = ( %.1f, %.1f )\n"
           "size = %.1f\n"
           "dir = %.1f\n"
           "is_moving = %d\n"
           "time_move = %.5f\n"
           "move_increment = ( %.5f, %.5f )\n",
           v.start_pos.x, v.start_pos.y, v.size, v.dir,
           v.is_moving, v.time_move, v.move_increment.x, v.move_increment.y);
}


Vector2 vec2_endpoints(Vec2 v)
{
    return Vector2Add(v.start_pos, (Vector2) { cosf(v.dir)*v.size, sinf(v.dir)*v.size });
}

Vec2 new_vector(Vector2 start_pos, float size, float dir, Color color)
{
    return (Vec2) {
        .start_pos = start_pos,
        .size = size,
        .dir = dir,
        .color = color
    };
}

void vec_move(Vec2 *v, Vector2 target_pos, float time)
{
    v->is_moving = true;
    v->target_pos = target_pos;
    Vector2 delta_pos = Vector2Subtract(target_pos, v->start_pos);
    v->move_increment = Vector2Scale(delta_pos, 1.0f/time);
    v->time_move = time;
}

void draw_vector(Vec2 v, Cartesian cart, PCamera camera)
{
    static float SIN_ANGLE_120 = sinf(120.0f * PI / 180.0f);
    static float COS_ANGLE_120 = cosf(120.0f * PI / 180.0f);
    static float SIN_ANGLE_240 = sinf(240.0f * PI / 180.0f);
    static float COS_ANGLE_240 = cosf(240.0f * PI / 180.0f);
    static float arrow_height = 0.2f;

    Vector2 end_pos = vec2_endpoints(v);

    Vector2 start_pos_screen = cartesian_to_screen(v.start_pos, cart, camera);
    Vector2 end_pos_screen = cartesian_to_screen(end_pos, cart, camera);
    DrawLineEx(start_pos_screen, end_pos_screen, 2.0f, v.color);

    float center_x = end_pos.x - arrow_height * cosf(v.dir);
    float center_y = end_pos.y - arrow_height * sinf(v.dir);

    Vector2 v1 = { end_pos.x, end_pos.y };
    v1 = cartesian_to_screen(v1, cart, camera);

    float dx = end_pos.x - center_x;
    float dy = end_pos.y - center_y;

    Vector2 v2 = {
        center_x + dx * COS_ANGLE_120 - dy * SIN_ANGLE_120,
        center_y + dx * SIN_ANGLE_120 + dy * COS_ANGLE_120
    };
    v2 = cartesian_to_screen(v2, cart, camera);

    Vector2 v3 = {
        center_x + dx * COS_ANGLE_240 - dy * SIN_ANGLE_240,
        center_y + dx * SIN_ANGLE_240 + dy * COS_ANGLE_240
    };
    v3 = cartesian_to_screen(v3, cart, camera);
    DrawTriangle(v1, v2, v3, v.color);
}
