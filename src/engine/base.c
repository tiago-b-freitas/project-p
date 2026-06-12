#include <stdio.h>
#include <math.h>
#include "engine.h"

void print_vector2(Vector2 v)
{
    printf("x = %.1f; y = %.1f\n", v.x, v.y);
}

Vector2 cartesian_to_screen(Vector2 pos, Engine *engine)
{
    return (Vector2) {
        .x =  ((pos.x - engine->camera.offset.x)*engine->cart.x_scale) + engine->cart.center_screen_x,
        .y = -((pos.y - engine->camera.offset.y)*engine->cart.y_scale) + engine->cart.center_screen_y,
    };
}

bool is_near(float f0, float f1)
{
    return fabsf(f0 - f1) < P_EPSILON;
}
