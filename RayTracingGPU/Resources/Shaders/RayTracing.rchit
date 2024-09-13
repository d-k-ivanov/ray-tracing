#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require
#include "Material.glsl"

layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;

#include "Scatter.glsl"
#include "Vertex.glsl"

const float PI = 3.1415926535897932384626433832795f;

hitAttributeEXT vec2       HitAttributes;
rayPayloadInEXT RayPayload Ray;

vec2 Mix(vec2 a, vec2 b, vec2 c, vec3 barycentrics)
{
    return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

vec3 Mix(vec3 a, vec3 b, vec3 c, vec3 barycentrics)
{
    return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
}

// Gram-Schmidt method
vec3 orthogonalize(in vec3 a, in vec3 b)
{
    // we assume that a is normalized
    return normalize(b - dot(a, b) * a);
}

// Return arcsine(x) given that .57 < x
float asin_tail(const float x)
{
    return (PI / 2) - ((x + 2.71745038) * x + 14.0375338) * (0.00440413551 * ((x - 8.31223679) * x + 25.3978882)) * sqrt(1 - x);
}

// Taken from apple libm source code
float portable_asinf(float x)
{
    const bool negate = (x < 0.0);
    if(abs(x) > 0.57)
    {
        const float ret = asin_tail(abs(x));
        return negate ? -ret : ret;
    }
    else
    {
        const float x2 = x * x;
        return float(x + (0.0517513789 * ((x2 + 1.83372748) * x2 + 1.56678128)) * x * (x2 * ((x2 - 1.48268414) * x2 + 2.05554748)));
    }
}

// Equivalent to acosf(dot(a, b)), but more numerically stable
// Taken from PBRT source code
float angle_between(const vec3 v1, const vec3 v2)
{
    if(dot(v1, v2) < 0)
    {
        return PI - 2 * portable_asinf(length(v1 + v2) / 2);
    }
    else
    {
        return 2 * portable_asinf(length(v2 - v1) / 2);
    }
}

float acos_positive_tail(const float x)
{
    return (((x + 2.71850395) * x + 14.7303705)) * (0.00393401226 * ((x - 8.60734272) * x + 27.0927486)) * sqrt(1 - x);
}

float acos_negative_tail(const float x)
{
    return PI - (((x - 2.71850395) * x + 14.7303705)) * (0.00393401226 * ((x + 8.60734272) * x + 27.0927486)) * sqrt(1 + x);
}

float portable_acosf(float x)
{
    if(x < -0.62)
    {
        return acos_negative_tail(x);
    }
    else if(x <= 0.62)
    {
        const float x2 = x * x;
        return (PI / 2) - x - (0.0700945929 * x * ((x2 + 1.57144082) * x2 + 1.25210774)) * (x2 * ((x2 - 1.53757966) * x2 + 1.89929986));
    }
    else
    {
        return acos_positive_tail(x);
    }
}

vec3 slerp(const vec3 start, const vec3 end, const float percent)
{
    // Dot product - the cosine of the angle between 2 vectors.
    float cos_theta = dot(start, end);
    // Clamp it to be in the range of Acos()
    // This may be unnecessary, but floating point
    // precision can be a fickle mistress.
    cos_theta = clamp(cos_theta, -1.0, 1.0);
    // Acos(dot) returns the angle between start and end,
    // And multiplying that by percent returns the angle between
    // start and the final result.
    const float theta        = portable_acosf(cos_theta) * percent;
    vec3        relative_vec = normalize(end - start * cos_theta);
    // Orthonormal basis
    // The final result.
    return start * cos(theta) + relative_vec * sin(theta);
}

// "Stratified Sampling of Spherical Triangles" https://www.graphics.cornell.edu/pubs/1995/Arv95c.pdf
// Based on https://www.shadertoy.com/view/4tGGzd
float SampleSphericalTriangle(const vec3 P, const vec3 p1, const vec3 p2, const vec3 p3, const vec2 randomUV, out vec3 out_dir)
{
    // setup spherical triangle
    const vec3 A = normalize(p1 - P), B = normalize(p2 - P), C = normalize(p3 - P);

    // calculate internal angles of spherical triangle: alpha, beta and gamma
    const vec3 BA = orthogonalize(A, B - A);
    const vec3 CA = orthogonalize(A, C - A);
    const vec3 AB = orthogonalize(B, A - B);
    const vec3 CB = orthogonalize(B, C - B);
    const vec3 BC = orthogonalize(C, B - C);
    const vec3 AC = orthogonalize(C, A - C);

    const float alpha = angle_between(BA, CA);
    const float beta  = angle_between(AB, CB);
    const float gamma = angle_between(BC, AC);

    const float area = alpha + beta + gamma - PI;
    if(area <= 0.00005f)
    {
        return 0.0;
    }

    // calculate arc lengths for edges of spherical triangle
    const float b = portable_acosf(clamp(dot(C, A), -1.0, 1.0));
    const float c = portable_acosf(clamp(dot(A, B), -1.0, 1.0));

    // Use one random variable to select the new area
    const float area_S = randomUV.x * area;

    // Save the sine and cosine of the angle delta
    const float p = sin(area_S - alpha);
    const float q = cos(area_S - alpha);

    // Compute the pair(u; v) that determines sin(beta_s) and cos(beta_s)
    const float u = q - cos(alpha);
    const float v = p + sin(alpha) * cos(c);

    // Compute the s coordinate as normalized arc length from A to C_s
    const float denom = (v * p + u * q) * sin(alpha);
    const float a1    = ((v * q - u * p) * cos(alpha) - v) / denom;
    const float s     = (1.0 / b) * portable_acosf(clamp(a1, -1.0, 1.0));

    // Compute the third vertex of the sub - triangle
    const vec3 C_s = slerp(A, C, s);

    // Compute the t coordinate using C_s and randomUV[1]
    const float b0     = dot(C_s, B);
    const float denom2 = portable_acosf(clamp(b0, -1.0, 1.0));
    const float c0     = 1.0 - randomUV.y * (1.0 - dot(C_s, B));
    const float t      = portable_acosf(clamp(c0, -1.0, 1.0)) / denom2;

    // Construct the corresponding point on the sphere.
    out_dir = slerp(B, C_s, t);

    // return pdf
    return (1.0 / area);
}

// random number generator **********
// from iq :)
float seed;    // seed initialized in main
float rnd() { return fract(sin(seed++) * 43758.5453123); }

void main()
{
    // Get the material.
    const uvec2    offsets      = Offsets[gl_InstanceCustomIndexEXT];
    const uint     indexOffset  = offsets.x;
    const uint     vertexOffset = offsets.y;
    const Vertex   v0           = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 0]);
    const Vertex   v1           = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 1]);
    const Vertex   v2           = UnpackVertex(vertexOffset + Indices[indexOffset + gl_PrimitiveID * 3 + 2]);
    const Material material     = Materials[v0.MaterialIndex];

    // Compute the ray hit point properties.
    const vec3 barycentrics = vec3(1.0 - HitAttributes.x - HitAttributes.y, HitAttributes.x, HitAttributes.y);
    const vec3 normal       = normalize(Mix(v0.Normal, v1.Normal, v2.Normal, barycentrics));
    const vec3 direction    = gl_WorldRayDirectionEXT;
    const vec2 texCoord     = Mix(v0.TexCoord, v1.TexCoord, v2.TexCoord, barycentrics);

    // vec3 worldPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    // vec3 worldPos = gl_WorldRayDirectionEXT;
    vec3 pos = v0.Position * barycentrics.x + v1.Position * barycentrics.y + v2.Position * barycentrics.z;
    // vec2 randomUV = vec2(RandomFloat(Ray.RandomSeed), RandomFloat(Ray.RandomSeed));
    vec2  randomUV = vec2(rnd(), rnd());
    vec3  out_dir;
    float pdfValue = SampleSphericalTriangle(pos, v0.Position, v1.Position, v2.Position, randomUV, out_dir);
    if(dot(-out_dir, normal) < 0.0)
    {
        // pdfValue = 0.0;
        pdfValue = 1.0f / (2.0f * 3.1415926535897932384626433832795);
    }

    // float cosTheta = dot(normalize(gl_WorldRayDirectionEXT), normal);
    // pdfValue = 1 / (2 * 3.1415926535897932384626433832795f) * (1 + cosTheta);
    // pdfValue = 1.0f / (2.0f * 3.1415926535897932384626433832795);

    Ray = Scatter(material, direction, normal, texCoord, gl_HitTEXT, Ray.RandomSeed, pdfValue);
}
