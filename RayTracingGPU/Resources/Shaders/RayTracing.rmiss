#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require
#include "RayPayload.glsl"
#include "UniformBufferObject.glsl"

layout(binding = 3) readonly uniform UniformBufferObjectStruct { UniformBufferObject Camera; };

layout(location = 0) rayPayloadInEXT RayPayload Ray;

void main()
{
    if(Camera.HasSky)
    {
        // Sky color
        const float t        = 0.5 * (normalize(gl_WorldRayDirectionEXT).y + 1);
        const vec3  skyColor = mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);

        Ray.ColorAndDistance = vec4(skyColor, -1);
        Ray.EmitColor        = vec4(skyColor, -1);
        // Ray.Pdf              = 0.0f;
        // Ray.SkipPdf          = true;
        // Ray.ScatterPdf       = 0.0f;
    }
    else
    {
        Ray.ColorAndDistance = vec4(0, 0, 0, -1);
        Ray.EmitColor        = vec4(0, 0, 0, -1);
        // Ray.Pdf              = 0.0f;
        // Ray.SkipPdf          = true;
        // Ray.ScatterPdf       = 0.0f;
    }
}
