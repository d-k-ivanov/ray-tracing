#include "Lambertian.h"

#include "Color.h"
#include "HitRecord.h"
#include "Ray.h"
#include "ScatterRecord.h"

#include <Math/CosinePDF.h>
#include <Math/ONB.h>

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