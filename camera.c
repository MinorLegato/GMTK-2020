
static void CameraUpdate(Camera* cam, f32 dt) {
    cam->current = v3_Add(
            v3_Lerp(cam->current, cam->target, 8.0f * dt),
            v3_Rand(-cam->shake, cam->shake));

    cam->shake -= dt;
}

