
static void RenderRect(v2 pos, v2 size, v4 col) {
    size = v2_Scale(size, 0.5f);
    
    glColor4ub(col.r, col.g, col.b, col.a);
    
    glBegin(GL_QUADS);
    glVertex2f(pos.x - size.x, pos.y - size.y);
    glVertex2f(pos.x + size.x, pos.y - size.y);
    glVertex2f(pos.x + size.x, pos.y + size.y);
    glVertex2f(pos.x - size.x, pos.y + size.y);
    glEnd();
}