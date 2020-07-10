
#define ENTITY_MAX  (1024)

typedef u32 EntityType;

enum EntityType_ {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_BULLET,
};

typedef struct Entity {
    i32         type;

    v2          pos;
    v2          vel;
    v2          acc;

    f32         rad;

    f32         life;
} Entity;

static void EntityFriction  (Entity* e, f32 friction);
static void EntityApply     (Entity* e, v2  force);
static void EntityPush      (Entity* e, v2  push);
static void EntityUpdate    (Entity* e, f32 dt);

typedef struct EntityManager {
    i32         count;
    Entity      array[ENTITY_MAX];
} EntityManager;

static void     AddEntity      (EntityManager* em, const Entity* e);
static void     RemoveEntity    (EntityManager* em, i32 index);
static Entity*  GetEntity       (EntityManager* em, i32 index);

