#include "Material.h"

namespace Render
{

Color3 Material::Emitted(double u, double v, const Math::Point3& p) const
{
    return {0, 0, 0};
}

Color3 Material::Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Math::Point3& p) const
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

}    // namespace Render
