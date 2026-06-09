#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <raymath.h>
#include "vec.h"

bool vec_is_equal(Vec2 v0, Vec2 v1)
{
    return is_near(v0.dir, v1.dir) && is_near(v0.size, v1.size);
}

void print_vec(Vec2 v)
{
    printf("label = %s\n"
           "start_pos = ( %.1f, %.1f )\n"
           "end_pos   = ( %.1f, %.1f )\n"
           "size = %.1f\n"
           "dir = %.1f\n"
           "is_moving = %d\n"
           "time_move = %.5f\n"
           "move_increment = ( %.5f, %.5f )\n",
           v.label,
           v.start_pos.x, v.start_pos.y, v.end_pos.x, v.end_pos.y,
           v.size, v.dir,
           v.is_moving, v.time_move,
           v.move_increment.x, v.move_increment.y);
}


Vector2 vec2_endpoints(Vec2 v)
{
    return Vector2Add(v.start_pos, (Vector2) { cosf(v.dir)*v.size, sinf(v.dir)*v.size });
}

Vec2 new_vector(Vector2 start_pos, float size, float dir, Color color, const char *label)
{
    Vec2 v = {
        .start_pos = start_pos,
        .size = size,
        .dir = dir,
        .color = color,
    };
    v.end_pos = vec2_endpoints(v);
    size_t l = strlen(label);
    // TODO: proper handle this error
    assert(l < MAX_SIZE_LABEL);
    strcpy(v.label, label);
    v.label[l] = '\0';
    return v;
}

void vec_move(Vec2 *v, Vector2 target_pos, float time)
{
    v->is_moving = true;
    v->target_pos = target_pos;
    Vector2 delta_pos = Vector2Subtract(target_pos, v->start_pos);
    v->move_increment = Vector2Scale(delta_pos, 1.0f/time);
    v->time_move = time;
}

void draw_vector(Vec2 v, Engine *engine)
{
    static float SIN_ANGLE_120 = sinf(120.0f * PI / 180.0f);
    static float COS_ANGLE_120 = cosf(120.0f * PI / 180.0f);
    static float SIN_ANGLE_240 = sinf(240.0f * PI / 180.0f);
    static float COS_ANGLE_240 = cosf(240.0f * PI / 180.0f);
    static float arrow_height = 0.2f;

    Vector2 start_pos_screen = cartesian_to_screen(v.start_pos, engine);
    Vector2 end_pos_screen = cartesian_to_screen(v.end_pos, engine);
    DrawLineEx(start_pos_screen, end_pos_screen, 2.0f, v.color);

    float center_x = v.end_pos.x - arrow_height * cosf(v.dir);
    float center_y = v.end_pos.y - arrow_height * sinf(v.dir);

    Vector2 v1 = { v.end_pos.x, v.end_pos.y };
    v1 = cartesian_to_screen(v1, engine);

    float dx = v.end_pos.x - center_x;
    float dy = v.end_pos.y - center_y;

    Vector2 v2 = {
        center_x + dx * COS_ANGLE_120 - dy * SIN_ANGLE_120,
        center_y + dx * SIN_ANGLE_120 + dy * COS_ANGLE_120
    };
    v2 = cartesian_to_screen(v2, engine);

    Vector2 v3 = {
        center_x + dx * COS_ANGLE_240 - dy * SIN_ANGLE_240,
        center_y + dx * SIN_ANGLE_240 + dy * COS_ANGLE_240
    };
    v3 = cartesian_to_screen(v3, engine);
    DrawTriangle(v1, v2, v3, v.color);
}

Vec2 vec_add(Vec2 v0, Vec2 v1, Color color)
{
    Vector2 v_end_pos = Vector2Add(v0.end_pos, v1.end_pos);
    Vector2 v_start_pos = Vector2Add(v0.start_pos, v1.start_pos);
    Vector2 diff = Vector2Subtract(v_end_pos, v_start_pos);
    return (Vec2) {
        .start_pos = v_start_pos,
        .dir       = atan2f(diff.y, diff.x),
        .size      = sqrtf(diff.x*diff.x+diff.y*diff.y),
        .color     = color
    };
}

Vec2 vec_sub(Vec2 v0, Vec2 v1, Color color)
{
    v1.dir += PI;
    return vec_add(v0, v1, color);

}
