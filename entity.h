
#define ENTITY_MAX  (1024)

typedef u32 EntityType;

enum EntityType_ {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_CORPSE,
    ENTITY_BULLET,
    ENTITY_AREA_DMG,
    ENTITY_COUNT
};

typedef u32 BulletPowers;

enum BulletPowers_ {
    POWERUP_NONE,
    POWERUP_SPEED,
    POWERUP_MELEE,
    POWERUP_EXPLOSIVE,
    POWERUP_FIRE,
    POWERUP_CHARGE,
    POWERUP_COUNT
};

static f32 powerup_cooldowns[] = {
    [POWERUP_NONE]       = 0.2f,
    [POWERUP_SPEED]      = 0.2f,
    [POWERUP_MELEE]      = 0.2f,
    [POWERUP_EXPLOSIVE]  = 0.5f,
    [POWERUP_FIRE]       = 0.2f,
    [POWERUP_CHARGE]     = 0.2f,
};

static v3 powerup_colors[] = {
    [POWERUP_NONE]      = { 0.0f, 0.0f, 0.0f },
    [POWERUP_SPEED]     = { 0.0f, 1.0f, 0.0f },
    [POWERUP_EXPLOSIVE] = { 1.0f, 0.0f, 0.0f },
    [POWERUP_FIRE]      = { 1.0f, 1.0f, 0.0f },
    [POWERUP_CHARGE]    = { 1.0f, 1.0f, 1.0f },
};

enum EntityFlags_ {
    ENTITY_FLAG_NONE    =   (0),
    ENTITY_FLAG_IMPACT  =   (1 << 1),
    ENTITY_FLAG_DAMAGE  =   (1 << 2),
};

typedef struct Entity {
    u32         type;
    u32         id;
    
    u32         flags;
    
    v2          pos;
    v2          vel;
    v2          acc;
    
    v2          aim;
    
    f32         rad;
    
    f32         life;
    
    i32         powerup;
    f32         cooldown;
    
    u32         owner_id;
} Entity;

static void EntityFriction  (Entity* e, f32 friction);
static void EntityApply     (Entity* e, v2  force);
static void EntityPush      (Entity* e, v2  push);
static void EntityUpdate    (Entity* e, f32 dt);
static b32  EntityIntersect (const Entity* a, const Entity* b);

typedef struct EntityManager {
    u32         next_id;
    
    i32         count;
    Entity      array[ENTITY_MAX];
} EntityManager;

static void     EntityAdd      (EntityManager* em, Entity* e);
static void     RemoveEntity    (EntityManager* em, i32 index);

