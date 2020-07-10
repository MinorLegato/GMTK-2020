
static void RenderRect(v2 pos, v2 scale, v4 col) {
    glColor4ub(col.r, col.g, col.b, col.a);
    glBegin(GL_QUADS);
    glVertex2f(pos.x - scale.x, pos.y - scale.y);
    glVertex2f(pos.x + scale.x, pos.y - scale.y);
    glVertex2f(pos.x + scale.x, pos.y + scale.y);
    glVertex2f(pos.x - scale.x, pos.y + scale.y);
    glEnd();
}
