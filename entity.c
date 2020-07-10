
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

static void AddEntity(EntityManager* em, const Entity* e) {
    em->array[em->count++] = *e;
}

static void RemoveEntity(EntityManager* em, i32 index) {
    em->array[index] = em->array[--em->count];
}

static Entity* GetEntity(EntityManager* em, i32 index) {
    return &em->array[index];
}

static void UpdateEntities(EntityManager* em, f32 dt) {
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                if (platform.key_down[GLFW_KEY_W]) EntityApply(e, (v2) {  0, +8 });
                if (platform.key_down[GLFW_KEY_S]) EntityApply(e, (v2) {  0, -8 });
                if (platform.key_down[GLFW_KEY_A]) EntityApply(e, (v2) { -8,  0 });
                if (platform.key_down[GLFW_KEY_D]) EntityApply(e, (v2) { +8,  0 });
                
                EntityFriction(e, 4.0f);
            } break;
        }
        
        EntityUpdate(e, dt);
    }
}

static void RenderEntities(EntityManager* em) {
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                RenderRect(e->pos, (v2) { e->rad, e->rad }, (v4) { 255, 100, 0, 255 });
            } break;
        }
    }
}

