#include "Dielectric.h"

#include "HitRecord.h"
#include "Ray.h"
#include "ScatterRecord.h"

bool Dielectric::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    attenuation                  = Color3(1.0, 1.0, 1.0);
    const double refractionRatio = rec.FrontFace ? (1.0 / m_Ir) : m_Ir;

    const Vector3 unitDirection = UnitVector(rIn.Direction());

    const double cosTheta = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
    const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    const bool cannotRefract = refractionRatio * sinTheta > 1.0;
    Vector3    direction;

    if(cannotRefract || Reflectance(cosTheta, refractionRatio) > Random::Double())
    {
        direction = Reflect(unitDirection, rec.Normal);
    }
    else
    {
        direction = Refract(unitDirection, rec.Normal, refractionRatio);
    }

    scattered = Ray(rec.P, direction, rIn.Time());

    return true;
}

// Produces black objects
bool Dielectric::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    attenuation                  = Color3(1.0, 1.0, 1.0);
    const double refractionRatio = rec.FrontFace ? (1.0 / m_Ir) : m_Ir;

    const Vector3 unitDirection = UnitVector(rIn.Direction());

    const double cosTheta = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
    const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    const bool cannotRefract = refractionRatio * sinTheta > 1.0;
    Vector3    direction;

    if(cannotRefract || Reflectance(cosTheta, refractionRatio) > Random::Double())
    {
        direction = Reflect(unitDirection, rec.Normal);
    }
    else
    {
        direction = Refract(unitDirection, rec.Normal, refractionRatio);
    }

    scattered = Ray(rec.P, direction, rIn.Time());
    pdf       = 1.0;

    return true;
}

bool Dielectric::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = Color3(1.0, 1.0, 1.0);
    srec.PDFPtr      = nullptr;
    srec.SkipPDF     = true;

    const double refractionRatio = rec.FrontFace ? (1.0 / m_Ir) : m_Ir;

    const Vector3 unitDirection = UnitVector(rIn.Direction());

    const double cosTheta = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
    const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    const bool cannotRefract = refractionRatio * sinTheta > 1.0;
    Vector3    direction;

    if(cannotRefract || Reflectance(cosTheta, refractionRatio) > Random::Double())
    {
        direction = Reflect(unitDirection, rec.Normal);
    }
    else
    {
        direction = Refract(unitDirection, rec.Normal, refractionRatio);
    }

    srec.SkipPDFRay = Ray(rec.P, direction, rIn.Time());

    return true;
}

double Dielectric::Reflectance(const double cosine, const double refIdx)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refIdx) / (1 + refIdx);
    r0      = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
