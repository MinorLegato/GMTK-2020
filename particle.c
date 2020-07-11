
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

static void ParticlesRender(const ParticleSystem* ps, const Map* map) {
    for(int i = 0; i < ps->count; i++) {
        const Particle* p       = &ps->particles[i];
        const Tile*     tile    = &map->tiles[(i32)p->pos.y][(i32)p->pos.x];

        v3 color = v3_Mul(p->col.rgb, tile->light);

        RenderRect(p->pos, 0.15f, (v2) { p->rad, p->rad }, (v4) { color.r, color.g, color.b, 1.0f });
    }
}
