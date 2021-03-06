
#define PARTICLE_MAX 1024 * 64

enum ParticleType {
    PARTICLE_NONE,
    PARTICLE_BLOOD,
    PARTICLE_COUNT
};

typedef struct Particle {
    u32     type;

    v3      pos;
    v3      vel;
    f32     rad;
    
    f32     life;
    f32     max_life;
    
    v4      col;
} Particle;

typedef struct ParticleSystem {
    i32         count;
    Particle    particles[PARTICLE_MAX];
} ParticleSystem;

static void ParticleAdd(ParticleSystem* ps, const Particle* p);

