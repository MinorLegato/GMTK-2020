
static void ParticleAdd(ParticleSystem* ps, const Particle* p) {
    ps->particles[ps->count++] = *p;
}

static void ParticleRemove(ParticleSystem* ps, i32 index) {
    ps->particles[index] = ps->particles[--ps->count];
}

static Particle CreateParticle(v2 pos, v2 vel, f32 rad, f32 life, v3 col) {
    Particle p = {
        .pos      = { .xy = pos },
        .vel      = { .xy = vel },
        .rad      = rad,
        .life     = life,
        .max_life = life,
        .col.rgb  = col
    };
    
    return p;
}

static void ParticleExplosion(ParticleSystem* ps, v2 pos, f32 max_rad, i32 amount, f32 max_speed) {
    for (int i = 0; i < amount; i++) {
        Particle p = CreateParticle(
                pos,
                v2_Rand(-max_speed, max_speed),
                fRand(0.03f, max_rad),
                fRand(0.5f, 1.0f),
                (v3) { fRand(0.7f, 1.0f), fRand(0.5f, 0.7f), 0.0f });

        ParticleAdd(ps, &p);
    }
}

static void BloodSpurter(ParticleSystem* ps, Entity* e, i32 amount) {
    for (int i = 0; i < amount; i++) {
        v3 color = { .r = fRand(0.3f, 1.0f) };

        Particle p = {
            .type       = PARTICLE_BLOOD,
            .pos        = { .xy = e->pos },
            .vel        = { .xy = v2_Add(v2_Scale(v2_Neg(e->aim), 2.0f), v2_Rand(-2.0f, 2.0f)) },
            .rad        = fRand(0.01f, 0.05f),
            .life       = fRand(0.5f, 1.0f),
            .max_life   = p.life,
            .col        = { .rgb = color },
        };

        ParticleAdd(ps, &p);
    }
}

static void BloodExplosion(ParticleSystem* ps, Entity* e, i32 amount) {
    for (int i = 0; i < amount; i++) {
        v3 color = { .r = fRand(0.3f, 1.0f) };
        
        Particle p = {
            .type       = PARTICLE_BLOOD,
            .pos        = { .xy = e->pos },
            .vel        = { .xy = v2_Rand(-3.0f, 3.0f) },
            .rad        = fRand(0.01f, 0.1f),
            .life       = fRand(0.5f, 1.0f),
            .max_life   = p.life,
            .col        = { .rgb = color },
        };

        ParticleAdd(ps, &p);
    }
}

static void ParticlesUpdate(GameState* gs, f32 t) {
    ParticleSystem*     ps      = &gs->particle_system;
    Map*                map     = &gs->map;

    for(int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        
        p->pos.x += p->vel.x * t;
        p->pos.y += p->vel.y * t;
        p->life  -= t;
        
        if (map->tiles[(i32)(p->pos.y + p->rad)][(i32)(p->pos.x)].type == TILE_WALL) { p->vel.y = 0.0f; p->pos.y = ceilf(p->pos.y)  - p->rad; }
        if (map->tiles[(i32)(p->pos.y - p->rad)][(i32)(p->pos.x)].type == TILE_WALL) { p->vel.y = 0.0f; p->pos.y = floorf(p->pos.y) + p->rad; }
        if (map->tiles[(i32)(p->pos.y)][(i32)(p->pos.x - p->rad)].type == TILE_WALL) { p->vel.x = 0.0f; p->pos.x = floorf(p->pos.x) + p->rad; }
        if (map->tiles[(i32)(p->pos.y)][(i32)(p->pos.x + p->rad)].type == TILE_WALL) { p->vel.x = 0.0f; p->pos.x = ceilf(p->pos.x)  - p->rad; }
        
        if (p->type == PARTICLE_BLOOD && p->life < 0.1f) {
            AddBlood(map, p->pos.x, p->pos.y, p->rad, p->col);
        }

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
        
        RenderRect(p->pos.xy, 0.5f, (v2) { p->rad, p->rad }, (v4) { color.r, color.g, color.b, p->life / p->max_life });
    }
}

