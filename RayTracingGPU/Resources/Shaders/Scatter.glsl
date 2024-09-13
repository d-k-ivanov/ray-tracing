#extension GL_EXT_nonuniform_qualifier : require

#include "Random.glsl"
#include "RayPayload.glsl"

// Polynomial approximation for reflectance by Christophe Schlick.
float Reflectance(const float cosine, const float refIdx)
{
    float r0 = (1 - refIdx) / (1 + refIdx);
    r0       = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

// Lambertian
RayPayload ScatterLambertian(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, float pdf)
{
    const bool isScattered      = dot(direction, normal) < 0;
    const vec4 texColor         = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
    const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
    const vec4 scatter          = vec4(normal + RandomOnHemisphere(seed, normal), isScattered ? 1 : 0);
    // const vec4 scatter          = vec4(normal + RandomCosineDirection(seed), isScattered ? 1 : 0);
    const vec4 emitColor = vec4(0);

    const float cosTheta = dot(normal, scatter.xyz);
    // const float scatterPdf = cosTheta < 0 ? 0.0 : cosTheta / 3.1415926535897932384626433832795;
    const float scatterPdf = 1 / (2 * 3.1415926535897932384626433832795);

    return RayPayload(colorAndDistance, emitColor, scatter, seed, pdf, false /*SkipPdf*/, scatterPdf);
}

// Metallic
RayPayload ScatterMetallic(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, float pdf)
{
    const vec3 reflected   = reflect(direction, normal);
    const bool isScattered = dot(reflected, normal) > 0;

    const vec4 texColor         = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
    const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
    const vec4 scatter          = vec4(reflected + m.Fuzziness * RandomOnHemisphere(seed, normal), isScattered ? 1 : 0);
    // const vec4  scatter    = vec4(reflected + m.Fuzziness * RandomCosineDirection(seed), isScattered ? 1 : 0);
    const vec4  emitColor  = vec4(0);
    const float scatterPdf = 0.0;

    return RayPayload(colorAndDistance, emitColor, scatter, seed, pdf, false /*SkipPdf*/, scatterPdf);
}

// Dielectric
RayPayload ScatterDieletric(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, float pdf)
{
    const float dot           = dot(direction, normal);
    const vec3  outwardNormal = dot > 0 ? -normal : normal;
    const float niOverNt      = dot > 0 ? m.RefractionIndex : 1 / m.RefractionIndex;
    const float cosine        = dot > 0 ? m.RefractionIndex * dot : -dot;

    const vec3  refracted   = refract(direction, outwardNormal, niOverNt);
    const float reflectProb = refracted != vec3(0) ? Reflectance(cosine, m.RefractionIndex) : 1;

    const vec4  texColor   = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
    const vec4  emitColor  = vec4(0);
    const float scatterPdf = 0.0;

    // const float pdf       = 1.0 / (4.0f * 3.1415926535897932384626433832795); iotropic

    return RandomFloat(seed) < reflectProb
               ? RayPayload(vec4(texColor.rgb, t), emitColor, vec4(reflect(direction, normal), 1), seed, pdf, true /*SkipPdf*/, scatterPdf)
               : RayPayload(vec4(texColor.rgb, t), emitColor, vec4(refracted, 1), seed, pdf, true /*SkipPdf*/, scatterPdf);
}

// Diffuse Light
RayPayload ScatterDiffuseLight(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed, float pdf)
{
    bool frontFace = dot(direction, normal) < 0 ? true : false;

    const vec4  colorAndDistance = vec4(m.Diffuse.rgb, t);
    const vec4  scatter          = vec4(1, 0, 0, 0);
    vec4        emitColor        = vec4(0);
    const float scatterPdf       = 0.0;

    if(frontFace)
    {
        emitColor = m.Diffuse;
    }

    return RayPayload(colorAndDistance, emitColor, scatter, seed, pdf, false /*SkipPdf*/, scatterPdf);
}

RayPayload Scatter(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, const float pdf)
{
    const vec3 normDirection = normalize(direction);

    switch(m.MaterialModel)
    {
        case MaterialLambertian:
            return ScatterLambertian(m, normDirection, normal, texCoord, t, seed, pdf);
        case MaterialMetallic:
            return ScatterMetallic(m, normDirection, normal, texCoord, t, seed, pdf);
        case MaterialDielectric:
            return ScatterDieletric(m, normDirection, normal, texCoord, t, seed, pdf);
        case MaterialDiffuseLight:
            return ScatterDiffuseLight(m, normDirection, normal, t, seed, pdf);
    }
}
