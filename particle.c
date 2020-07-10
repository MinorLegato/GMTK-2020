
static void ParticleAdd(ParticleSystem* ps, const Particle* p) {
    ps->particles[ps->count++] = *p;
}

static void ParticleRemove(ParticleSystem* ps, i32 index) {
    ps->particles[index] = ps->particles[--ps->count];
}

static void ParticlesUpdate(ParticleSystem* ps, f32 t) {
    for(int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        p->pos.x += p->vel.x * t;
        p->pos.y += p->vel.y * t;
        p->life  -= t;
        if(p->life <= 0.0f) {
            ParticleRemove(ps, i);
            i--;
        }
    }
}

static void ParticlesRender(ParticleSystem* ps) {
    for(int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        RenderRect(p->pos, p->size, p->col);
    }
}
