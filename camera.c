
static void CameraUpdate(Camera* cam, f32 dt) {
    cam->current = v3_Add(
            v3_Lerp(cam->current, cam->target, 4.0f * dt),
            v3_Rand(-cam->shake, cam->shake));

    cam->shake = fClamp(cam->shake - dt, 0.0f, 1.0f);
}

