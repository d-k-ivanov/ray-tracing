#include "DiffuseLight.h"

#include "HitRecord.h"


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

