#pragma once

#include "Material.h"
#include "Texture.h"

class Isotropic final : public Material
{
public:
    explicit Isotropic(const Color3& color)
        : m_Albedo(std::make_shared<SolidColor>(color))
    {
    }

    explicit Isotropic(const std::shared_ptr<Texture>& texture)
        : m_Albedo(texture)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

    double ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const override;

private:
    std::shared_ptr<Texture> m_Albedo;
};
