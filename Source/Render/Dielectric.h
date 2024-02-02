#pragma once

#include "Material.h"

class Dielectric final : public Material
{
public:
    explicit Dielectric(const double indexOfRefraction)
        : m_Ir(indexOfRefraction)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

private:
    double m_Ir;    // Index of Refraction

    static double Reflectance(double cosine, double refIdx);
};
