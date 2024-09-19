#extension GL_EXT_nonuniform_qualifier : require

#include "Common.glsl"
#include "Random.glsl"
#include "RayPayload.glsl"

// Lambertian
RayPayload ScatterLambertian(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, float pdf)
{
    const bool isScattered      = dot(direction, normal) < 0;
    const vec4 texColor         = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
    const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);

    vec3 tangent, bitangent;
    vec4 scatter;
    CreateCoordinateSystem(normal, tangent, bitangent);

    // scatter       = vec4(normal + RandomInUnitSphere(seed), isScattered ? 1 : 0);
    // scatter       = vec4(RandomOnHemisphere0(seed, normal), isScattered ? 1 : 0);
    // scatter          = vec4(AlignWithNormal(RandomOnHemisphere1(seed), normal), isScattered ? 1 : 0);
    scatter          = vec4(RandomOnHemisphere2(seed, tangent, bitangent, normal), isScattered ? 1 : 0);
    // scatter          = vec4(AlignWithNormal(RandomCosineDirection(seed), normal), isScattered ? 1 : 0);
    // scatter          = vec4(RandomCosineDirectionN(seed, normal), isScattered ? 1 : 0);
    // scatter          = vec4(AlignWithNormal(RandomUnitVector(seed), normal), isScattered ? 1 : 0);

    const vec4 emitColor   = vec4(0);
    float       scatterPdf;
    float cosTheta;

    vec3 scatterTransform = scatter.x * bitangent + scatter.y * tangent + scatter.z * normalize(normal);

    // cosTheta = dot(normal, scatterTransform);
    cosTheta = dot(normal, scatter.xyz);

    scatterPdf    = cosTheta < 0 ? 0.0 : cosTheta / M_PI;
    // scatterPdf = cosTheta < 0 ? 0.0 : cosTheta / M_TWO_PI;
    // scatterPdf = cosTheta < 0 ? 0.0 : 1 / M_TWO_PI;
    // scatterPdf = 1 / M_TWO_PI;
    // scatterPdf = 1.0f;

    // scatter = vec4(scatterTransform, isScattered ? 1 : 0);
    return RayPayload(colorAndDistance, emitColor, scatter, seed, pdf, false /*SkipPdf*/, scatterPdf);
}

// Metallic
RayPayload ScatterMetallic(const Material m, const vec3 direction, const vec3 normal, const vec2 texCoord, const float t, inout uint seed, float pdf)
{
    const vec3 reflected   = reflect(direction, normal);
    const bool isScattered = dot(reflected, normal) > 0;

    vec3 tangent, bitangent;
    CreateCoordinateSystem(normal, tangent, bitangent);

    const vec4 texColor         = m.DiffuseTextureId >= 0 ? texture(TextureSamplers[nonuniformEXT(m.DiffuseTextureId)], texCoord) : vec4(1);
    const vec4 colorAndDistance = vec4(m.Diffuse.rgb * texColor.rgb, t);
    const vec4 scatter          = vec4(reflected + m.Fuzziness * RandomInUnitSphere(seed), isScattered ? 1 : 0);
    // const vec4 scatter       = vec4(reflected + m.Fuzziness * RandomOnHemisphere0(seed, normal), isScattered ? 1 : 0);
    // const vec4 scatter       = vec4(reflected + m.Fuzziness * RandomOnHemisphere1(seed), isScattered ? 1 : 0);
    // const vec4 scatter       = vec4(reflected + m.Fuzziness * RandomOnHemisphere2(seed, tangent, bitangent, normal), isScattered ? 1 : 0);
    // const vec4  scatter      = vec4(reflected + m.Fuzziness * RandomCosineDirection(seed), isScattered ? 1 : 0);

    const vec4  emitColor  = vec4(0);
    const float scatterPdf = 1.0;

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

    // const float pdf       = 1.0 / (4.0f * M_PI); isotropic

    return RandomFloat(seed) < reflectProb
               ? RayPayload(vec4(texColor.rgb, t), emitColor, vec4(reflect(direction, normal), 1), seed, pdf, true /*SkipPdf*/, scatterPdf)
               : RayPayload(vec4(texColor.rgb, t), emitColor, vec4(refracted, 1), seed, pdf, true /*SkipPdf*/, scatterPdf);
}

// Diffuse Light
RayPayload ScatterDiffuseLight(const Material m, const vec3 direction, const vec3 normal, const float t, inout uint seed, float pdf)
{
    bool frontFace = dot(direction, normal) < 0 ? true : false;

    // vec4  colorAndDistance = vec4(m.Diffuse.rgb, t);
    vec4  colorAndDistance = vec4(0, 0, 0, t);
    vec4  scatter          = vec4(1, 0, 0, 0);
    vec4  emitColor        = vec4(0);
    float scatterPdf       = 0.0;

    if(frontFace)
    {
        emitColor        = vec4(m.Diffuse.rgb, 1.0);
        colorAndDistance = vec4(1, 1, 1, t);
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
