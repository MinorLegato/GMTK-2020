
static void RenderRect(v2 pos, f32 z, v2 rad, v4 col) {
    glColor4f(col.r, col.g, col.b, col.a);
    
    glBegin(GL_QUADS);

    glVertex3f(pos.x - rad.x, pos.y - rad.y, z);
    glVertex3f(pos.x + rad.x, pos.y - rad.y, z);
    glVertex3f(pos.x + rad.x, pos.y + rad.y, z);
    glVertex3f(pos.x - rad.x, pos.y + rad.y, z);

    glEnd();
}

static void RenderSquare(v2 pos, f32 rad, v4 col) {
    glColor4f(col.r, col.g, col.b, col.a);
    
    glBegin(GL_QUADS);

    glVertex2f(pos.x - rad, pos.y - rad);
    glVertex2f(pos.x + rad, pos.y - rad);
    glVertex2f(pos.x + rad, pos.y + rad);
    glVertex2f(pos.x - rad, pos.y + rad);

    glEnd();
}


static void RenderBox(v3 pos, v3 scale, v4 col) {
    glColor4f(col.r, col.g, col.b, col.a);
    
    glBegin(GL_QUADS);

    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z);
    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z);

    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z);
    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z);
    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z);

    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z);
    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z);

    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z);
    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z);

    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z);
    glVertex3f(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z);
    glVertex3f(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z);

    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z);
    glVertex3f(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z);
    glVertex3f(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z);

    glEnd();
}
