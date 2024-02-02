#include "Metal.h"

#include "HitRecord.h"
#include "Ray.h"
#include "ScatterRecord.h"

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
