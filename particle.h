
#define PARTICLE_MAX 2048

typedef struct Particle {
    v2  pos;
    v2  size;
    v2  vel;
    v4  col;
    f32 life;
} Particle;

typedef struct ParticleSystem {
    Particle particles[PARTICLE_MAX];
    i32 count;
} ParticleSystem;