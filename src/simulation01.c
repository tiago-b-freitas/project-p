#include <stdlib.h>
#include <math.h>

#include "engine.h"

void simulation(Engine *engine)
{
    Vec2 v0 = new_vector((Vector2) { 0, 0 }, sqrtf(50.0f),  -PI/4.0f, GREEN, "v0");
    Vec2 v1 = new_vector((Vector2) { 2, -2 }, sqrtf(70.0f),  -PI/3.0f, BLUE, "v0");
    da_append(engine->v_ar, v0);
    da_append(engine->v_ar, v1);
}

int main()
{
    engine(simulation);
    return 0;
}

    /* print_vec(v0); */
    /* Vec2 v1 = new_vector((Vector2) { 0, 0 }, sqrtf(15.0f),  PI - PI/6.0f, RED); */
    /* da_push(v_ar, v1); */
    /* v_ar.items[v_ar.count-1].dir += PI; */
    /* vec_move(&(v_ar.items[v_ar.count-1]), vec2_endpoints(v_ar.items[v_ar.count-2]), 3.0f); */

    /* Vec2 v2 = vec_sub(v0, v1, RAYWHITE); */
    /* da_push(v_ar, v2); */
