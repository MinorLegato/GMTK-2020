
#define PARTICLE_MAX 1024 * 64

typedef struct Particle {
    v2      pos;
    v2      vel;
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

