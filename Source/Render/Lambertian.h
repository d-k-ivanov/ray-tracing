#pragma once

#include "Material.h"
#include "SolidColor.h"
#include "Texture.h"

class Lambertian final : public Material
{
public:
    explicit Lambertian(const Color3& color)
        : m_Albedo(std::make_shared<SolidColor>(color))
    {
    }

    explicit Lambertian(const std::shared_ptr<Texture>& texture)
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