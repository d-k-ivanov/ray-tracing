#pragma once

#include "Material.h"

class Metal final : public Material
{
public:
    explicit Metal(const Color3& color, const double fuzz)
        : m_Albedo(color)
        , m_Fuzz(fuzz < 1 ? fuzz : 1)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

private:
    Color3 m_Albedo;
    double m_Fuzz;
};
