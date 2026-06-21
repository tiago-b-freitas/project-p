#include <stdlib.h>
#include <math.h>

#include "engine.h"

#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "../src/engine/nob.h"

void simulation(Engine *engine)
{
    /* Object obj1 = {0}; */
    /* obj1.pos = (Vector2) { 0, 0 }; */
    /* obj1.size = 10; */
    /* obj1.color = RAYWHITE; */
    /* obj1.velocity.y = 20.0f; */
    /* obj1.velocity.x = 5.0f; */
    /* da_append(engine->v_obj, obj1); */

    Object obj2 = {0};
    obj2.pos = (Vector2) { 0, 0 };
    obj2.size = 10;
    obj2.color = RED;
    obj2.velocity.y = 20.0f;
    obj2.velocity.x = 3.0f;
    da_append(engine->v_obj, obj2);
}

int main()
{
    app(simulation);
    return 0;
}

    /* print_vec(v0); */
    /* Vec2 v1 = new_vector((Vector2) { 0, 0 }, sqrtf(15.0f),  PI - PI/6.0f, RED); */
    /* da_push(v_ar, v1); */
    /* v_ar.items[v_ar.count-1].dir += PI; */
    /* vec_move(&(v_ar.items[v_ar.count-1]), vec2_endpoints(v_ar.items[v_ar.count-2]), 3.0f); */

    /* Vec2 v2 = vec_sub(v0, v1, RAYWHITE); */
    /* da_push(v_ar, v2); */
