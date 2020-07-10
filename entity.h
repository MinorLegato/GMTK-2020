
typedef u32 EntityType;

enum EntityType_ {
    ENTITY_NONE,
    ENTITY_PLAYER,
};

typedef struct Entity {
    i32         type;
    v2          pos;
    v2          vel;
    f32         rad;
} Entity;

