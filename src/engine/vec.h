#ifndef VEC_H
#define VEC_H

#include "base.h"

Vec2 new_vector(Vector2 start_pos, float size, float dir, Color color, const char *label);
bool vec_is_equal(Vec2 v0, Vec2 v1);
void print_vec(Vec2 v);
Vector2 vec2_endpoints(Vec2 v);
void vec_move(Vec2 *v, Vector2 target_pos, float time);
void draw_vector(Vec2 v, Engine *engine);
Vec2 vec_sub(Vec2 v0, Vec2 v1, Color color);
Vec2 vec_add(Vec2 v0, Vec2 v1, Color color);

#endif // VEC_H
