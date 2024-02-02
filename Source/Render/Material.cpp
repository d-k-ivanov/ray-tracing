#include "Material.h"

#include <Math/CosinePDF.h>
#include <Math/ONB.h>
#include <Math/SpherePDF.h>

Color3 Material::Emitted(double u, double v, const Point3& p) const
{
    return {0, 0, 0};
}

Color3 Material::Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Point3& p) const
{
    return {0, 0, 0};
}

bool Material::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    return false;
}

double Material::ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const
{
    return 0;
}

bool Lambertian::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    auto scatterDirection = rec.Normal + RandomUnitVector();

    // Catch degenerate scatter direction
    if(scatterDirection.NearZero())
    {
        scatterDirection = rec.Normal;
    }
    scattered   = Ray(rec.P, scatterDirection, rIn.Time());
    attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    return true;
}

bool Lambertian::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    ONB uvw;
    uvw.BuildFromW(rec.Normal);
    const auto scatterDirection = uvw.Local(RandomCosineDirection());
    scattered                   = Ray(rec.P, UnitVector(scatterDirection), rIn.Time());
    attenuation                 = m_Albedo->Value(rec.U, rec.V, rec.P);
    pdf                         = DotProduct(uvw.W(), scattered.Direction()) / Pi;
    return true;
}

bool Lambertian::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    srec.PDFPtr      = std::make_shared<CosinePDF>(rec.Normal);
    srec.SkipPDF     = false;
    return true;
}

double Lambertian::ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const
{
    const auto cosTheta = DotProduct(rec.Normal, UnitVector(scattered.Direction()));
    return cosTheta < 0 ? 0 : cosTheta / Pi;
}

bool Metal::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    const Vector3 reflected = Reflect(UnitVector(rIn.Direction()), rec.Normal);
    scattered               = Ray(rec.P, reflected + m_Fuzz * RandomUnitVector(), rIn.Time());
    attenuation             = m_Albedo;
    return (DotProduct(scattered.Direction(), rec.Normal) > 0);
}

// Produces black objects
bool Metal::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    const Vector3 reflected = Reflect(UnitVector(rIn.Direction()), rec.Normal);
    scattered               = Ray(rec.P, reflected + m_Fuzz * RandomUnitVector(), rIn.Time());
    attenuation             = m_Albedo;
    pdf                     = 1.0;
    return (DotProduct(scattered.Direction(), rec.Normal) > 0);
}

bool Metal::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation        = m_Albedo;
    srec.PDFPtr             = nullptr;
    srec.SkipPDF            = true;
    const Vector3 reflected = Reflect(UnitVector(rIn.Direction()), rec.Normal);
    srec.SkipPDFRay         = Ray(rec.P, reflected + m_Fuzz * RandomUnitVector(), rIn.Time());
    return true;
}

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

bool DiffuseLight::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    return false;
}

bool DiffuseLight::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    return false;
}

Color3 DiffuseLight::Emitted(const double u, const double v, const Point3& p) const
{
    return m_Emit->Value(u, v, p);
}

Color3 DiffuseLight::Emitted(const Ray& rIn, const HitRecord& rec, const double u, const double v, const Point3& p) const
{
    if(!rec.FrontFace)
    {
        return {0, 0, 0};
    }
    return m_Emit->Value(u, v, p);
}

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    scattered   = Ray(rec.P, RandomUnitVector(), rIn.Time());
    attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    return true;
}

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    scattered   = Ray(rec.P, RandomUnitVector(), rIn.Time());
    attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    pdf         = 1 / (4 * Pi);
    return true;
}

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    srec.PDFPtr      = std::make_shared<SpherePDF>();
    srec.SkipPDF     = false;
    return true;
}

double Isotropic::ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const
{
    return 1 / (4 * Pi);
}
