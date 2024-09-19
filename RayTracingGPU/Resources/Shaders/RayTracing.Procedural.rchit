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
layout(binding = 9) readonly buffer SphereArray { vec4[] Spheres; };

#include "Common.glsl"
#include "Scatter.glsl"
#include "Vertex.glsl"

hitAttributeEXT vec4       Sphere;
rayPayloadInEXT RayPayload Ray;

vec2 GetSphereTexCoord(const vec3 point)
{
    const float phi   = atan(point.x, point.z);
    const float theta = asin(point.y);
    const float pi    = M_PI;

    return vec2((phi + pi) / (2 * pi), 1 - (theta + pi / 2) / pi);
}

void main()
{
    // Get the material.
    const uvec2    offsets      = Offsets[gl_InstanceCustomIndexEXT];
    const uint     indexOffset  = offsets.x;
    const uint     vertexOffset = offsets.y;
    const Vertex   v0           = UnpackVertex(vertexOffset + Indices[indexOffset]);
    const Material material     = Materials[v0.MaterialIndex];

    // Compute the ray hit point properties.
    const vec4  sphere   = Spheres[gl_InstanceCustomIndexEXT];
    const vec3  center   = sphere.xyz;
    const float radius   = sphere.w;
    const vec3  point    = gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;
    const vec3  normal   = (point - center) / radius;
    const vec2  texCoord = GetSphereTexCoord(normal);

    float cosThetaMax = sqrt(1 - radius * radius / dot(center - point, center - point));
    float solidAngle  = 2 * M_PI * (1 - cosThetaMax);
    float pdfValue    = 1 / solidAngle;

    if(isnan(pdfValue))
    {
        // pdfValue = 1 / (4 * M_PI);
        // pdfValue = 1 / (2 * M_PI);
        pdfValue = 1.0f;
    }
    // pdfValue = 1 / (2 * M_PI);
    // ppdfValue = 1.0f;

    Ray = Scatter(material, gl_WorldRayDirectionEXT, normal, texCoord, gl_HitTEXT, Ray.RandomSeed, pdfValue);
}
