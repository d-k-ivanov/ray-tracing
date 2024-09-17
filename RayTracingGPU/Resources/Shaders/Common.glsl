#ifndef Common

    #define LocalToWorld(v, t, b, n) (mat3((t), (b), (n)) * (v))
    #define WorldToLocal(v, T, B, N) ((v) * mat3((T), (B), (N)))

    #define M_PI 3.14159265358979323846                // pi
    #define M_TWO_PI 6.283185307179586476925           // 2*pi
    #define M_PI_2 1.57079632679489661923              // pi/2
    #define M_PI_4 0.785398163397448309616             // pi/4
    #define M_1_PI 0.318309886183790671538             // 1/pi
    #define M_2_PI 0.636619772367581343076             // 2/pi
    #define M_1_OVER_TWO_PI 0.159154943091895335768    // 1/ (2*pi)

    #define INFINITY 1e32
    #define EPSILON 1e-3

    #define NEARzero 1e-35f
    #define AlmostZero(x) ((x) > -NEARzero && (x) < NEARzero)
    #define AlmostLessThanZero(a) ((a).x + (a).y + (a).z < NEARzero)
    #define AlmostGreaterZero(a) ((a).x + (a).y + (a).z >= NEARzero)
    #define AlmostLessThanZeroABS(a) (abs((a).x) + abs((a).y) + abs((a).z) < NEARzero)
    #define AlmostGreaterZeroABS(a) (abs((a).x) + abs((a).y) + abs((a).z) >= NEARzero)

    #define Mix(a, b, c, barycentrics) ((a) * (barycentrics).x + (b) * (barycentrics).y + (c) * (barycentrics).z)
    #define Luminance(rgb) (dot((rgb), vec3(0.212671F, 0.715160F, 0.072169F)))

    #define Saturate(x) (clamp((x), 0.0F, 1.0F))

float pow5(float x) { return x * x * x * x * x; }
float pow4(float x) { return x * x * x * x; }

// Polynomial approximation for reflectance by Christophe Schlick.
// https://www.photometric.io/blog/improving-schlicks-approximation/
float Reflectance(const float cosine, const float refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 *= r0;
    return r0 + (1 - cosine - r0) * pow4(1 - cosine);
}

vec3 AlignWithNormal(vec3 ray, vec3 up)
{
    vec3 right   = normalize(cross(up, vec3(0.0072f, 1.0f, 0.0034f)));
    vec3 forward = cross(right, up);
    return LocalToWorld(ray, right, up, forward);
}

void ONBAlignWithNormal(vec3 up, out vec3 right, out vec3 forward)
{
    right   = normalize(cross(up, vec3(0.0072f, 1.0f, 0.0034f)));
    forward = cross(right, up);
}

// Return the tangent and binormal from the incoming normal
void CreateCoordinateSystem(in vec3 N, out vec3 Nt, out vec3 Nb)
{
    if(abs(N.x) > abs(N.y))
        Nt = vec3(N.z, 0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
    else
        Nt = vec3(0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
    Nb = cross(N, Nt);
}


    #define Common
#endif
