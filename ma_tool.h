#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

typedef float   f32;
typedef double  f64;

typedef char        i8;
typedef short       i16;
typedef int         i32;
typedef long long   i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef unsigned char       b8;
typedef unsigned short      b16;
typedef unsigned int        b32;
typedef unsigned long long  b64;

#ifndef false 
#define false   (0)
#endif

#ifndef true 
#define true   (!false)
#endif

#ifndef PI
#define PI  (3.14159265359f)
#endif

#define ARRAY_COUNT(array) (sizeof (array) / sizeof (array[0]))

typedef union v2 {
    struct {
        f32     x;
        f32     y;
    };

    f32 array[2];
} v2;

typedef union v3 {
    struct {
        f32     x;
        f32     y;
        f32     z;
    };

    struct {
        v2      xy;
        f32     _z;
    };

    struct {
        f32     r;
        f32     g;
        f32     b;
    };

    f32 array[3];
} v3;

typedef union v4 {
    struct {
        f32     x;
        f32     y;
        f32     z;
        f32     w;
    };

    struct {
        f32     r;
        f32     g;
        f32     b;
        f32     a;
    };

    struct {
        v3      rgb;
        f32     _a;
    };

    f32 array[4];
} v4;

typedef union m2 {
    struct {
        v2  x;
        v2  y;
    };

    f32 array[4];
} m2;

typedef union m3 {
    struct {
        v3  x;
        v3  y;
        v3  z;
    };

    f32 array[9];
} m3;

typedef union m4 {
    struct {
        v4  x;
        v4  y;
        v4  z;
        v4  w;
    };

    f32 array[16];
} m4;

typedef struct Rect2 {
    v2      min;
    v2      max;
} Rect2;

static f32 rsqrtf(f32 n) {
    return n == 0.0f? 0.0f : 1.0f / sqrtf(n);
}

static f32 fLerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}

static f32 fUnLerp(f32 a, f32 b, f32 t) {
    return (t - a) / (b - a);
}

static f32 fSmoothStep(f32 n) {
	if (n < 0.0f) { return 0.0f; }
	if (n < 1.0f) { return (3.0f * n * n - 2.0f * n * n * n); }

	return 1.0f;
}

static f32 fShortAngleDist(f32 a, f32 b) {
    f32 max = 2.0f * PI;
    f32 da  = fmodf(b - a, max);
    return fmodf(2.0f * da, max) - da;
}

static f32 fLerpAngle(f32 a, f32 b, f32 t) {
    return a + fShortAngleDist(a, b) * t;
}

static f32 fSnap(f32 f, f32 step_size) {
	if (f > 0)  { return (f32)((int)(f / step_size + 0.5f)) * step_size; }
    else        { return (f32)((int)(f / step_size - 0.5f)) * step_size; }
}

static f32 fSpline(f32 f, f32 a, f32 b, f32 c, f32 d) {
	f32 i = 1.0f - f;
	return ((d * f + c * i) * f + (c * f + b * i) * i) * f + ((c * f + b * i) * f + (b * f + a * i) * i) * i;
}

static f32 fMin(f32 a, f32 b) {
    return a < b? a : b;
}

static f32 fMax(f32 a, f32 b) {
    return a < b? b : a;
}

static int iMin(int a, int b) {
    return a < b? a : b;
}

static int iMax(int a, int b) {
    return a < b? b : a;
}

static f32 fClamp(f32 n, f32 min, f32 max) {
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

static f32 fClampMin(f32 n, f32 min) {
    return n < min? min : n;
}

static f32 fClampMax(f32 n, f32 max) {
    return n > max? max : n;
}

static int iClamp(int n, int min, int max) {
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

static int iClampMin(int n, int min) {
    return n < min? min : n;
}

static int iClampMax(int n, int max) {
    return n > max? max : n;
}

// v2:
static v2 v2_Neg(v2 a) {
    v2 out = { -a.x, -a.y };
    return out;
}

static v2 v2_Add(v2 a, v2 b) {
    v2 out = { a.x + b.x, a.y + b.y };
    return out;
}

static v2 v2_Sub(v2 a, v2 b) {
    v2 out = { a.x - b.x, a.y - b.y };
    return out;
}

static v2 v2_Mul(v2 a, v2 b) {
    v2 out = { a.x * b.x, a.y * b.y };
    return out;
}

static v2 v2_Scale(v2 a, f32 s) {
    v2 out = { a.x * s, a.y * s };
    return out;
}

static f32 v2_Dot(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

static f32 v2_LenSq(v2 v) {
    return v2_Dot(v, v);
}

static f32 v2_Len(v2 v) {
    return sqrtf(v2_Dot(v, v));
}

static f32 v2_DistSq(v2 a, v2 b) {
    return v2_LenSq(v2_Sub(b, a));
}

static f32 v2_Dist(v2 a, v2 b) {
    return sqrtf(v2_DistSq(a, b));
}

static v2 v2_Norm(v2 v) {
    return v2_Scale(v, rsqrtf(v2_Dot(v, v)));
}

static v2 v2_Min(v2 a, v2 b) {
    v2 out = {
        (a.x < b.x? a.x : b.x),
        (a.y < b.y? a.y : b.y)
    };
    return out;
}

static v2 v2Max(v2 a, v2 b) {
    v2 out = {
        (a.x > b.x? a.x : b.x),
        (a.y > b.y? a.y : b.y)
    };
    return out;
}

static v2 v2_Lerp(v2 a, v2 b, f32 t) {
    v2 out = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y)
    };
    return out;
}

static f32 v2_GetAngle(v2 a, v2 b) {
    f32 det = a.x * b.y - b.x * a.y;
    f32 dot = a.x * b.x + a.y * b.y;
    
    return atan2f(det, dot);
}

static v2 v2_Spline(f32 f, v2 a, v2 b, v2 c, v2 d) {
	f32 i = 1.0f - f;

    v2 out = {
	    ((d.x * f + c.x * i) * f + (c.x * f + b.x * i) * i) * f + ((c.x * f + b.x * i) * f + (b.x * f + a.x * i) * i) * i,
	    ((d.y * f + c.y * i) * f + (c.y * f + b.y * i) * i) * f + ((c.y * f + b.y * i) * f + (b.y * f + a.y * i) * i) * i,
    };

    return out;
}

static int v2_CircleIntersect(v2 p0, f32 r0, v2 p1, f32 r1) {
    f32 dx = p1.x - p0.x;
    f32 dy = p1.y - p0.y;

    f32 r = r0 + r1;

    return (dx * dx + dy * dy) < (r * r);
}

static int v2_SegmentIsIntersectingCircle(v2 start, v2 end, v2 pos, f32 rad) {
    v2 a = { start.x - pos.x, start.y - pos.y };
    v2 b = { end.x - start.x, end.y - start.y };

    if ((a.x * a.x + a.y * a.y) > (b.x * b.x + b.y * b.y))
        return 0;

    v2 seg = { end.x - start.x, end.y - start.y };
    v2 cir = { pos.x - start.x, pos.y - start.y };

    f32 dot_sc = seg.x * cir.x + seg.y * cir.y;

    if (dot_sc < 0.0f)
        return 0;

    f32 proj = dot_sc / (seg.x * seg.x + seg.y * seg.y);

    seg.x *= proj;
    seg.y *= proj;

    seg.x = seg.x - cir.x;
    seg.y = seg.y - cir.y;

    return (seg.x * seg.x + seg.y * seg.y) < (rad * rad);
}

// v3:
static v3 v3_Neg(v3 a) {
    v3 out = { -a.x, -a.y, -a.z };
    return out;
}

static v3 v3_Add(v3 a, v3 b) {
    v3 out = { a.x + b.x, a.y + b.y, a.z + b.z };
    return out;
}

static v3 v3_Sub(v3 a, v3 b) {
    v3 out = { a.x - b.x, a.y - b.y, a.z - b.z };
    return out;
}

static v3 v3_Mul(v3 a, v3 b) {
    v3 out = { a.x * b.x, a.y * b.y, a.z * b.z };
    return out;
}

static v3 v3_Scale(v3 a, f32 s) {
    v3 out = { a.x * s, a.y * s, a.z * s };
    return out;
}

static f32 v3_Dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static f32 v3_LenSq(v3 v) {
    return v3_Dot(v, v);
}

static f32 v3_Len(v3 v) {
    return sqrtf(v3_Dot(v, v));
}

static f32 v3_DistSq(v3 a, v3 b) {
    return v3_LenSq(v3_Sub(b, a));
}

static f32 v3_Dist(v3 a, v3 b) {
    return sqrtf(v3_DistSq(a, b));
}

static v3 v3_Norm(v3 v) {
    return v3_Scale(v, rsqrtf(v3_Dot(v, v)));
}

static v3 v3_Min(v3 a, v3 b) {
    v3 out = {
        (a.x < b.x? a.x : b.x),
        (a.y < b.y? a.y : b.y),
        (a.z < b.z? a.z : b.z)
    };
    return out;
}

static v3 v3_Max(v3 a, v3 b) {
    v3 out = {
        (a.x > b.x? a.x : b.x),
        (a.y > b.y? a.y : b.y),
        (a.z > b.z? a.z : b.z)
    };
    return out;
}

static v3 v3_Lerp(v3 a, v3 b, f32 t) {
    v3 out = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z)
    };

    return out;
}

static v3 v3_Cross(v3 a, v3 b) {
    v3 r;

	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;

    return r;
}

static v3 v3_Spline(f32 f, v3 a, v3 b, v3 c, v3 d) {
	f32 i = 1.0f - f;

    v3 out = {
        ((d.x * f + c.x * i) * f + (c.x * f + b.x * i) * i) * f + ((c.x * f + b.x * i) * f + (b.x * f + a.x * i) * i) * i,
        ((d.y * f + c.y * i) * f + (c.y * f + b.y * i) * i) * f + ((c.y * f + b.y * i) * f + (b.y * f + a.y * i) * i) * i,
        ((d.z * f + c.z * i) * f + (c.z * f + b.z * i) * i) * f + ((c.z * f + b.z * i) * f + (b.z * f + a.z * i) * i) * i
    };

    return out;
}

#define U8_TO_F32_COLOR_MUL (0.00390625f)

// v4:
static v4 v4_FromRGB(u8 red, u8 green, u8 blue) {
    return (v4) {
        .r = red   * U8_TO_F32_COLOR_MUL,
        .g = green * U8_TO_F32_COLOR_MUL,
        .b = blue  * U8_TO_F32_COLOR_MUL,
        .a = 1.0f
    };
}

static v4 v4_FromRGBA(u8 red, u8 green, u8 blue, u8 alpha) {
    return (v4) {
        .r = red   * U8_TO_F32_COLOR_MUL,
        .g = green * U8_TO_F32_COLOR_MUL,
        .b = blue  * U8_TO_F32_COLOR_MUL,
        .a = alpha * U8_TO_F32_COLOR_MUL,
    };
}

static v4 v4_Neg(v4 a) {
    v4 out = { -a.x, -a.y, -a.z, -a.w };
    return out;
}

static v4 v4_Add(v4 a, v4 b) {
    v4 out = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return out;
}

static v4 v4_Sub(v4 a, v4 b) {
    v4 out = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return out;
}

static v4 v4_Mul(v4 a, v4 b) {
    v4 out = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    return out;
}

static v4 v4_Scale(v4 a, f32 s) {
    v4 out = { a.x * s, a.y * s, a.z * s, a.w * s };
    return out;
}

static f32 v4_Dot(v4 a, v4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static f32 v4_LenSq(v4 v) {
    return v4_Dot(v, v);
}

static f32 v4_Len(v4 v) {
    return sqrtf(v4_Dot(v, v));
}

static f32 v4_DistSq(v4 a, v4 b) {
    return v4_LenSq(v4_Sub(b, a));
}

static f32 v4_Dist(v4 a, v4 b) {
    return sqrtf(v4_DistSq(a, b));
}

static v4 v4_Norm(v4 v) {
    return v4_Scale(v, rsqrtf(v4_Dot(v, v)));
}

static v4 v4_Min(v4 a, v4 b) {
    v4 out = {
        (a.x < b.x? a.x : b.x),
        (a.y < b.y? a.y : b.y),
        (a.z < b.z? a.z : b.z),
        (a.w < b.w? a.w : b.w)
    };
    return out;
}

static v4 v4_Max(v4 a, v4 b) {
    v4 out = {
        (a.x > b.x? a.x : b.x),
        (a.y > b.y? a.y : b.y),
        (a.z > b.z? a.z : b.z),
        (a.w > b.w? a.w : b.w)
    };
    return out;
}

static v4 v4_Lerp(v4 a, v4 b, f32 t) {
    v4 out = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        a.w + t * (b.w - a.w)
    };
    return out;
}

static v4 v4_Cross(v4 a, v4 b) {
    v4 out = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        1.0f
    };
    return out;
}

static v4 v4_Spline(f32 f, v4 a, v4 b, v4 c, v4 d) {
	f32 i = 1.0f - f;

    v4 out = {
	    ((d.x * f + c.x * i) * f + (c.x * f + b.x * i) * i) * f + ((c.x * f + b.x * i) * f + (b.x * f + a.x * i) * i) * i,
	    ((d.y * f + c.y * i) * f + (c.y * f + b.y * i) * i) * f + ((c.y * f + b.y * i) * f + (b.y * f + a.y * i) * i) * i,
	    ((d.z * f + c.z * i) * f + (c.z * f + b.z * i) * i) * f + ((c.z * f + b.z * i) * f + (b.z * f + a.z * i) * i) * i,
	    ((d.w * f + c.w * i) * f + (c.w * f + b.w * i) * i) * f + ((c.w * f + b.w * i) * f + (b.w * f + a.w * i) * i) * i
    };

    return out;
}

// m2:
static m2 m2_Mul(m2 a, m2 b) {
    m2 out = {
        a.array[0] * b.array[0] + a.array[2] * b.array[1],
        a.array[1] * b.array[0] + a.array[3] * b.array[1],
        a.array[0] * b.array[2] + a.array[2] * b.array[3],
        a.array[1] * b.array[2] + a.array[3] * b.array[3]
    };
    return out;
}

static v2 m2_Mulv(m2 R, v2 v) {
    v2 out = {
        R.array[0] * v.x + R.array[2] * v.y,
        R.array[1] * v.x + R.array[3] * v.y
    };
    return out;
}

static m2 m2_Identity(void) {
    m2 out = { 1.0f, 0.0f, 0.0f, 1.0f };
    return out;
}

static m2 m2_Rotate(f32 angle) {
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    m2 out = { c, s, -s, c };

    return out;
}

static m2 m2_Scale(f32 sx, f32 sy) {
    m2 out = { sx, 0.0f, 0.0f, sy };
    return out;
}

// m3:
static m3 m3_Mul(m3 a, m3 b) {
    m3 out = {
        a.array[0] * b.array[0] + a.array[3] * b.array[1]  + a.array[6] * b.array[2],
        a.array[1] * b.array[0] + a.array[4] * b.array[1]  + a.array[7] * b.array[2],
        a.array[2] * b.array[0] + a.array[5] * b.array[1]  + a.array[8] * b.array[2],

        a.array[0] * b.array[3] + a.array[3] * b.array[4]  + a.array[6] * b.array[5],
        a.array[1] * b.array[3] + a.array[4] * b.array[4]  + a.array[7] * b.array[5],
        a.array[2] * b.array[3] + a.array[5] * b.array[4]  + a.array[8] * b.array[5],

        a.array[0] * b.array[6] + a.array[3] * b.array[7]  + a.array[6] * b.array[8],
        a.array[1] * b.array[6] + a.array[4] * b.array[7]  + a.array[7] * b.array[8],
        a.array[2] * b.array[6] + a.array[5] * b.array[7]  + a.array[8] * b.array[8]
    };
    return out;
}

static v3 m3_Mulv(m3 M, v3 v) {
    v3 out = {
        M.array[0] * v.array[0] + M.array[3] * v.array[1] + M.array[6] * v.array[2],
        M.array[1] * v.array[0] + M.array[4] * v.array[1] + M.array[7] * v.array[2],
        M.array[2] * v.array[0] + M.array[5] * v.array[1] + M.array[8] * v.array[2]
    };
    return out;
}

static m3 m3_Identity(void) {
    m3 out = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    return out;
}

static m3 m3_Rotate(v3 axis, f32 angle) {
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    f32 k = 1.0f - c;

    v3 sa   = { s * axis.x, s * axis.y, s * axis.z };
    v3 omca = { k * axis.x, k * axis.y, k * axis.z };

    m3 out = {
        omca.x * axis.x + c,
        omca.x * axis.y - sa.z,
        omca.x * axis.z + sa.y,

        omca.y * axis.x + sa.z,
        omca.y * axis.y + c,
        omca.y * axis.z - sa.x,

        omca.z * axis.x - sa.y,
        omca.z * axis.y + sa.x,
        omca.z * axis.z + c
    };
    return out;
}

static m3 m3_Euler(f32 yaw, f32 pitch, f32 roll) {
    f32 cy = cosf(yaw);
    f32 sy = sinf(yaw);
    f32 cp = cosf(pitch);
    f32 sp = sinf(pitch);
    f32 cr = cosf(roll);
    f32 sr = sinf(roll);

    m3 out = {
        cy * cp,
        sy * cp,
        -sp,

        cy * sp * sr - sy * cr,
        sy * sp * sr + cy * cr,
        cp * sr,

        cy * sp * cr + sy * sr,
        sy * sp * cr - cy * sr,
        cp * cr,
    };

    return out;
}

// m4:
static m4 m4_Mul(m4 a, m4 b) {
    m4 out = {
        a.array[0] * b.array[0]  + a.array[4] * b.array[1]  + a.array[8]  * b.array[2]  + a.array[12] * b.array[3],
        a.array[1] * b.array[0]  + a.array[5] * b.array[1]  + a.array[9]  * b.array[2]  + a.array[13] * b.array[3],
        a.array[2] * b.array[0]  + a.array[6] * b.array[1]  + a.array[10] * b.array[2]  + a.array[14] * b.array[3],
        a.array[3] * b.array[0]  + a.array[7] * b.array[1]  + a.array[11] * b.array[2]  + a.array[15] * b.array[3],

        a.array[0] * b.array[4]  + a.array[4] * b.array[5]  + a.array[8]  * b.array[6]  + a.array[12] * b.array[7],
        a.array[1] * b.array[4]  + a.array[5] * b.array[5]  + a.array[9]  * b.array[6]  + a.array[13] * b.array[7],
        a.array[2] * b.array[4]  + a.array[6] * b.array[5]  + a.array[10] * b.array[6]  + a.array[14] * b.array[7],
        a.array[3] * b.array[4]  + a.array[7] * b.array[5]  + a.array[11] * b.array[6]  + a.array[15] * b.array[7],

        a.array[0] * b.array[8]  + a.array[4] * b.array[9]  + a.array[8]  * b.array[10] + a.array[12] * b.array[11],
        a.array[1] * b.array[8]  + a.array[5] * b.array[9]  + a.array[9]  * b.array[10] + a.array[13] * b.array[11],
        a.array[2] * b.array[8]  + a.array[6] * b.array[9]  + a.array[10] * b.array[10] + a.array[14] * b.array[11],
        a.array[3] * b.array[8]  + a.array[7] * b.array[9]  + a.array[11] * b.array[10] + a.array[15] * b.array[11],

        a.array[0] * b.array[12] + a.array[4] * b.array[13] + a.array[8]  * b.array[14] + a.array[12] * b.array[15],
        a.array[1] * b.array[12] + a.array[5] * b.array[13] + a.array[9]  * b.array[14] + a.array[13] * b.array[15],
        a.array[2] * b.array[12] + a.array[6] * b.array[13] + a.array[10] * b.array[14] + a.array[14] * b.array[15],
        a.array[3] * b.array[12] + a.array[7] * b.array[13] + a.array[11] * b.array[14] + a.array[15] * b.array[15]
    };
    return out;
}

static v4 m4_Mulv(m4 M, v4 v) {
    v4 out = {
        M.array[0] * v.array[0] + M.array[4] * v.array[1] + M.array[8]  * v.array[2] + M.array[12] * v.array[3],
        M.array[1] * v.array[0] + M.array[5] * v.array[1] + M.array[9]  * v.array[2] + M.array[13] * v.array[3],
        M.array[2] * v.array[0] + M.array[6] * v.array[1] + M.array[10] * v.array[2] + M.array[14] * v.array[3],
        M.array[3] * v.array[0] + M.array[7] * v.array[1] + M.array[11] * v.array[2] + M.array[15] * v.array[3]
    };
    return out;
}

static m4 m4_Identity(void) {
    m4 out = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return out;
}

static m4 m4_Transpose(m4 N) {
    m4 out = {
        N.array[0],   N.array[1],   N.array[2],   N.array[3],
	    N.array[4],   N.array[5],   N.array[6],   N.array[7],
	    N.array[8],   N.array[9],   N.array[10],  N.array[11],
	    N.array[12],  N.array[13],  N.array[14],  N.array[15]
    };
    return out;
}

static m4 m4_Translate(f32 x, f32 y, f32 z) {
    m4 out = {
        1.0f,   0.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,   0.0f,
        x,      y,      z,      1.0f
    };
    return out;
}

static m4 m4_RotateX(f32 angle) {
    f32 s = sinf(angle);
	f32 c = cosf(angle);

    m4 out = {
        1.0f,   0.0f,   0.0f,   0.0f,
        0.0f,      c,      s,   0.0f,
        0.0f,     -s,      c,   0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };
    return out;
}

static m4 m4_RotateY(f32 angle) {
    f32 s = sinf(angle);
	f32 c = cosf(angle);

    m4 out = {
           c,   0.0f,      s,   0.0f,
        0.0f,   1.0f,   0.0f,   0.0f,
          -s,   0.0f,      c,   0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };
    return out;
}

static m4 m4_RotateZ(f32 angle) {
	f32 s = sinf(angle);
	f32 c = cosf(angle);

    m4 out = {
           c,      s,   0.0f,   0.0f,
          -s,      c,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };
    return out;
}

static m4 m4_Rotate(v3 axis, f32 angle) {
    f32 sa[3];
    f32 omca[3];

    f32 cosv      = cosf(angle);
    f32 sinv      = sinf(angle);
    f32 inv_cosv  = 1.0f - cosv;

    sa[0] = axis.x * sinv;
    sa[1] = axis.y * sinv;
    sa[2] = axis.z * sinv;

    omca[0] = axis.x * inv_cosv;
    omca[1] = axis.y * inv_cosv;
    omca[2] = axis.z * inv_cosv;

    m4 out = {
        omca[0] * axis.x + cosv,   omca[0] * axis.y - sa[0],  omca[0] * axis.z + sa[1],  0.0f,
        omca[1] * axis.x + sa[2],  omca[1] * axis.y + cosv,   omca[1] * axis.z - sa[0],  0.0f,
        omca[2] * axis.x - sa[1],  omca[2] * axis.y + sa[0],  omca[2] * axis.z + cosv,   0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };

    return out;
}

static m4 m4_Frustum(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) {
    m4 out = {
        2.0f * n / (r - l),
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        2.0f * n / (t - b),
        0.0f,
        0.0f,

        (r + l) / (r - l),
        (t + b) / (t - b),
        -(f + n) / (f - n),
        -1.0f,

        0.0f,
        0.0f,
        -2.0f * (f * n) / (f - n),
        0.0f
    };
    return out;
}

static m4 m4_Ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) {
    m4 out = {
        2.0f / (r - l),
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        2.0f / (t - b),
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        -2.0f / (f - n),
        0.f,

        -(r + l) / (r - l),
        -(t + b) / (t - b),
        -(f + n) / (f - n),
        1.0f
    };
    return out;
}

static m4 m4_Perspective(f32 y_fov, f32 aspect, f32 n, f32 f) {
    f32 a = 1.0f / tanf(y_fov / 2.0f);

    m4 out = {
        a / aspect,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        a,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        -((f + n) / (f - n)),
        -1.0f,

        0.0f,
        0.0f,
        -((2.f * f * n) / (f - n)),
        0.0f
    };
    return out;
}

static m4 m4_LookAt(v3 eye, v3 center, v3 up) {
    v3 f = v3_Norm(v3_Sub(center, eye));
    v3 s = v3_Norm(v3_Cross(f, up));
	v3 t = v3_Cross(s, f);

    m4 M;

	M.array[0]  =  s.x;
	M.array[1]  =  t.x;
	M.array[2]  = -f.x;
	M.array[3]  =   0.0f;

	M.array[4]  =  s.y;
	M.array[5]  =  t.y;
	M.array[6]  = -f.y;
	M.array[7]  =   0.0f;

	M.array[8]  =  s.z;
	M.array[9]  =  t.z;
	M.array[10] = -f.z;
	M.array[11] =   0.0f;

    M.array[12] = -(M.array[0] * eye.x + M.array[4] * eye.y + M.array[8]  * eye.z);
    M.array[13] = -(M.array[1] * eye.x + M.array[5] * eye.y + M.array[9]  * eye.z);
    M.array[14] = -(M.array[2] * eye.x + M.array[6] * eye.y + M.array[10] * eye.z);
    M.array[15] = -(M.array[3] * eye.x + M.array[7] * eye.y + M.array[11] * eye.z - 1.0f);

    return M;
}

// ============================================== RANDOM GENERATOR =========================================== //
// random number generator: xorshf96

typedef struct RndGen {
    u32    x;
    u32    y;
    u32    z;
} RndGen;

static RndGen default_rnd_gen = { 123456789u, 362436069u, 521288629u };

static u32 RndGenNext(RndGen* g) {
    g->x  ^= g->x << 16;
    g->x  ^= g->x >> 5;
    g->x  ^= g->x << 1;

    u32 t = g->x;

    g->x  = g->y;
    g->y  = g->z;
    g->z  = t ^ g->x ^ g->y;

    return g->z;
}

static int iRand(int min, int max) {
    return min + RndGenNext(&default_rnd_gen) % (max - min);
}

static f32 fRand(f32 min, f32 max) {
    return min + ((f32)RndGenNext(&default_rnd_gen) / (f32)0xFFFFFFFF) * (max - min); 
}

static void f2_Rand(f32 *out, f32 min, f32 max) {
    out[0] = fRand(-1.0f, 1.0f);
    out[1] = fRand(-1.0f, 1.0f);

    f32 k = rsqrtf(out[0] * out[0] + out[1] * out[1]) * fRand(min, max);

    out[0] *= k;
    out[1] *= k;
}

static void f2_RandUnit(f32 *out) {
    out[0] = fRand(-1.0f, 1.0f);
    out[1] = fRand(-1.0f, 1.0f);

    f32 k = rsqrtf(out[0] * out[0] + out[1] * out[1]);

    out[0] *= k;
    out[1] *= k;
}

static void f3_Rand(f32 *out, f32 min, f32 max) {
    out[0] = fRand(-1.0f, 1.0f);
    out[1] = fRand(-1.0f, 1.0f);
    out[2] = fRand(-1.0f, 1.0f);

    f32 k = rsqrtf(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]) * fRand(min, max);

    out[0] *= k;
    out[1] *= k;
    out[2] *= k;
}

static void f3_RandUnit(f32 *out) {
    out[0] = fRand(-1.0f, 1.0f);
    out[1] = fRand(-1.0f, 1.0f);
    out[2] = fRand(-1.0f, 1.0f);

    f32 k = rsqrtf(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);

    out[0] *= k;
    out[1] *= k;
    out[2] *= k;
}

static void f2_AddRand(f32 *out, f32* u, f32 min, f32 max) {
    f32 r[2];
    f2_Rand(r, min, max);

    out[0] = u[0] + r[0];
    out[1] = u[1] + r[1];
}

static void f3_AddRand(f32 *out, f32* u, f32 min, f32 max) {
    f32 r[3];
    f3_Rand(r, min, max);

    out[0] = u[0] + r[0];
    out[1] = u[1] + r[1];
    out[2] = u[2] + r[2];
}

static v2 v2_Rand(f32 min, f32 max) {
    v2 out;
    f2_Rand(out.array, min, max);
    return out;
}

static v2 v2_RandUnit(void) {
    v2 out;
    f2_RandUnit(out.array);
    return out;
}

static v3 v3_Rand(f32 min, f32 max) {
    v3 out;
    f3_Rand(out.array, min, max);
    return out;
}

static v3 v3_RandUnit(void) {
    v3 out;
    f3_RandUnit(out.array);
    return out;
}

// ================================================== PLATFORM ========================================== //

struct {
    b32             close;

    i32             width;
    i32             height;
    f32             aspect;

    GLFWwindow*     window;

    f32             time_delta;
    f32             time_total;

    u8              key_down        [GLFW_KEY_LAST + 1];
    u8              key_pressed     [GLFW_KEY_LAST + 1];
    u8              key_released    [GLFW_KEY_LAST + 1];
    
    v2              mouse_position;

    u8              mouse_down      [GLFW_MOUSE_BUTTON_LAST + 1];
    u8              mouse_pressed   [GLFW_MOUSE_BUTTON_LAST + 1];
    u8              mouse_released  [GLFW_MOUSE_BUTTON_LAST + 1];
} platform;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    switch (action) {
        case GLFW_PRESS: {
            platform.key_down[key]      = true;
            platform.key_pressed[key]   = true;
        } break;
        case GLFW_RELEASE: {
            platform.key_down[key]      = false;
            platform.key_released[key]  = true;
        } break;
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;

    switch (action) {
        case GLFW_PRESS: {
            platform.mouse_down[button]      = true;
            platform.mouse_pressed[button]   = true;
        } break;
        case GLFW_RELEASE: {
            platform.mouse_down[button]      = false;
            platform.mouse_released[button]  = true;
        } break;
    }
}

void PlatformInit(const char* title, i32 width, i32 height) {
    glfwInit();

    platform.window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(platform.window);

    glfwSetKeyCallback(platform.window, KeyCallback);
    glfwSetMouseButtonCallback(platform.window, MouseButtonCallback);

    glfwSetTime(0.0f);
}

void PlatformUpdate(void) {
    platform.time_total += platform.time_delta = glfwGetTime(); glfwSetTime(0.0f);

    glfwGetWindowSize(platform.window, &platform.width, &platform.height);
    platform.aspect = (f32)platform.width / (f32)platform.height;

    glViewport(0, 0, platform.width, platform.height);

    for (int i = 0; i < ARRAY_COUNT(platform.key_pressed);  ++i) platform.key_pressed[i]  = false;
    for (int i = 0; i < ARRAY_COUNT(platform.key_released); ++i) platform.key_released[i] = false;

    for (int i = 0; i < ARRAY_COUNT(platform.mouse_pressed);  ++i) platform.mouse_pressed[i]  = false;
    for (int i = 0; i < ARRAY_COUNT(platform.mouse_released); ++i) platform.mouse_released[i] = false;

    if (glfwWindowShouldClose(platform.window)) {
        platform.close = true;
    }

    {
        f64 mouse_x = 0.0f, mouse_y = 0.0f;
        glfwGetCursorPos(platform.window, &mouse_x, &mouse_y);

        platform.mouse_position = (v2) { mouse_x, mouse_y };
    }

    glfwSwapBuffers(platform.window);
    glfwPollEvents();
}

static v3 ToWorldPosition(i32 x, int y) {
    GLint       viewport[4];
    GLdouble    modelview[16];
    GLdouble    projection[16];
    GLfloat     winX, winY, winZ;
    GLdouble    posX, posY, posZ;
 
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
 
    winX = (f32)x;
    winY = (f32)viewport[3] - (f32)y;

    glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
 
    return (v3) { posX, posY, posZ };
}
