#extension GL_EXT_control_flow_attributes : require

#include "Common.glsl"

// Generates a seed for a random number generator from 2 inputs plus a backoff
// https://github.com/nvpro-samples/optix_prime_baking/blob/332a886f1ac46c0b3eea9e89a59593470c755a0e/random.h
// https://github.com/nvpro-samples/vk_raytracing_tutorial_KHR/tree/master/ray_tracing_jitter_cam
// https://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm
uint InitRandomSeed(uint val0, uint val1)
{
    uint v0 = val0, v1 = val1, s0 = 0;

    [[unroll]]
    for(uint n = 0; n < 16; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}

uint RandomInt(inout uint seed)
{
    // LCG values from Numerical Recipes
    return (seed = 1664525 * seed + 1013904223);
}

float RandomFloat(inout uint seed)
{
    //// Float version using bitmask from Numerical Recipes
    // const uint one = 0x3f800000;
    // const uint msk = 0x007fffff;
    // return uintBitsToFloat(one | (msk & (RandomInt(seed) >> 9))) - 1;

    // Faster version from NVIDIA examples; quality good enough for our use case.
    return (float(RandomInt(seed) & 0x00FFFFFF) / float(0x01000000));
}

vec2 RandomInUnitDisk(inout uint seed)
{
    for(;;)
    {
        const vec2 p = 2 * vec2(RandomFloat(seed), RandomFloat(seed)) - 1;
        if(dot(p, p) < 1)
        {
            return p;
        }
    }
}

vec3 RandomInUnitSphere(inout uint seed)
{
    for(;;)
    {
        const vec3 p = 2 * vec3(RandomFloat(seed), RandomFloat(seed), RandomFloat(seed)) - 1;
        if(dot(p, p) < 1)
        {
            return p;
        }
    }
}

vec3 RandomUnitVector(inout uint seed)
{
    return normalize(RandomInUnitSphere(seed));
}

vec3 RandomOnHemisphere0(inout uint seed, const vec3 normal)
{
    const vec3 onUnitSphere = RandomUnitVector(seed);
    if(dot(onUnitSphere, normal) > 0.0)    // In the same hemisphere as the normal
    {
        return onUnitSphere;
    }
    else
    {
        return -onUnitSphere;
    }
}

vec3 RandomOnHemisphere1(inout uint seed)
{
    const vec2  u         = vec2(RandomFloat(seed), RandomFloat(seed));
    float       phi       = 2.0f * M_PI * u.x;
    float       cos_phi   = cos(phi);
    float       sin_phi   = sin(phi);
    float       cos_theta = sqrt(u.y);
    float       sin_theta = sqrt(1.0f - cos_theta * cos_theta);
    return vec3(sin_theta * cos_phi, cos_theta, sin_theta * sin_phi);
}

vec3 RandomOnHemisphere2(inout uint seed, in vec3 x, in vec3 y, in vec3 z)
{
    float r1 = RandomFloat(seed);
    float r2 = RandomFloat(seed);
    float sq = sqrt(r1);

    vec3 direction = vec3(cos(2 * M_PI * r2) * sq, sin(2 * M_PI * r2) * sq, sqrt(1.0f - r1));
    direction = direction.x * x + direction.y * y + direction.z * z;
    return direction;
}

vec3 RandomCosineDirection(inout uint seed)
{
    const float pi = M_PI;
    const float r1 = RandomFloat(seed);
    const float r2 = RandomFloat(seed);

    const float phi = 2 * pi * r1;
    const float x   = cos(phi) * sqrt(r2);
    const float y   = sin(phi) * sqrt(r2);
    const float z   = sqrt(1 - r2);

    return vec3(x, y, z);
}
