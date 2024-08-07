#include "Isotropic.h"

#include "Color.h"
#include "HitRecord.h"
#include "Ray.h"
#include "ScatterRecord.h"

#include <Math/SpherePDF.h>

namespace Render
{

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const
{
    scattered   = Ray(rec.P, Math::RandomUnitVector(), rIn.Time());
    attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    return true;
}

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const
{
    scattered   = Ray(rec.P, Math::RandomUnitVector(), rIn.Time());
    attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    pdf         = 1 / (4 * Math::Pi);
    return true;
}

bool Isotropic::Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
    srec.PDFPtr      = std::make_shared<Math::SpherePDF>();
    srec.SkipPDF     = false;
    return true;
}

double Isotropic::ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const
{
    return 1 / (4 * Math::Pi);
}

}    // namespace Render
