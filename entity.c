
static void EntityApply(Entity* e, v2 force) {
    e->acc.x += force.x;
    e->acc.y += force.y;
}

static void EntityPush(Entity* e, v2  push) {
    e->vel.x += push.x;
    e->vel.y += push.y;
}

static void EntityFriction(Entity* e, f32 friction) {
    e->acc.x -= e->vel.x * friction;
    e->acc.y -= e->vel.y * friction;
}

static void EntityUpdate(Entity* e, f32 dt) {
    e->vel.x += e->acc.x * dt;
    e->vel.y += e->acc.y * dt;
    e->pos.x += e->vel.x * dt;
    e->pos.y += e->vel.y * dt;
    e->acc.x = 0.0f;
    e->acc.y = 0.0f;
}

static b32 EntityIntersect(const Entity* a, const Entity* b) {
    f32 dx  = a->pos.x - b->pos.x;
    f32 dy  = a->pos.y - b->pos.y;
    f32 r   = a->rad + b->rad;

    return (dx * dx + dy * dy) < (r * r);
}

static void EntityAdd(EntityManager* em, const Entity* e) {
    em->array[em->count++] = *e;
}

static void EntityRemove(EntityManager* em, i32 index) {
    em->array[index] = em->array[--em->count];
}

