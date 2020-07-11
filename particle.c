
static void ParticleAdd(ParticleSystem* ps, const Particle* p) {
    ps->particles[ps->count++] = *p;
}

static void ParticleRemove(ParticleSystem* ps, i32 index) {
    ps->particles[index] = ps->particles[--ps->count];
}

static Particle CreateParticle(v2 pos, v2 vel, f32 rad, f32 life, v3 col) {
    Particle p = {
        .pos      = pos,
        .vel      = vel,
        .rad      = rad,
        .life     = life,
        .max_life = life,
        .col.rgb  = col
    };

    return p;
}

static void ParticleExplosion(ParticleSystem* ps, v2 pos, f32 max_rad, i32 amount, f32 max_speed) {
    for (int i = 0; i < amount; i++) {
        Particle p = CreateParticle(pos, v2_Rand(-max_speed, max_speed), fRand(0.03f, max_rad),
                                    fRand(0.5f, 1.0f), v3_Rand(0.0f, 1.0f));
        ParticleAdd(ps, &p);
    }
}

static void BloodSpurter(ParticleSystem* ps, Entity* e, i32 amount) {
    for (int i = 0; i < amount; i++) {
        v3 color = { .r = fRand(0.3f, 1.0f) };
        Particle p = CreateParticle(e->pos, v2_Add(v2_Scale(v2_Neg(e->aim), 2.0f), v2_Rand(-2.0f, 2.0f)),
                                    fRand(0.01f, 0.05f), fRand(0.5f, 1.0f), color);
        ParticleAdd(ps, &p);
    }
}

static void BloodExplosion(ParticleSystem* ps, Entity* e, i32 amount) {
    for (int i = 0; i < amount; i++) {
        v3 color = { .r = fRand(0.3f, 1.0f) };
        Particle p = CreateParticle(e->pos, v2_Rand(-3.0f, 3.0f), fRand(0.01f, 0.1f),
                                    fRand(0.5f, 1.0f), color);
        ParticleAdd(ps, &p);
    }
}

static void ParticlesUpdate(ParticleSystem* ps, f32 t) {
    for(int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        
        p->pos.x += p->vel.x * t;
        p->pos.y += p->vel.y * t;
        p->life  -= t;
        
        if (p->life <= 0.0f) {
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
        
        RenderRect(p->pos, 0.5f, (v2) { p->rad, p->rad }, (v4) { color.r, color.g, color.b, p->life / p->max_life });
    }
}
