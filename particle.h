
#define PARTICLE_MAX 2048

typedef struct Particle {
    v2      pos;
    v2      vel;
    f32     rad;
    f32     life;
    v4      col;
} Particle;

typedef struct ParticleSystem {
    i32         count;
    Particle    particles[PARTICLE_MAX];
} ParticleSystem;
