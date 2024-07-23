#pragma once

#include "Material.h"
#include "SolidColor.h"
#include "Texture.h"

namespace Render
{

class DiffuseLight final : public Material
{
public:
    explicit DiffuseLight(const std::shared_ptr<Texture>& texture)
        : m_Emit(texture)
    {
    }

    explicit DiffuseLight(const Color3& color)
        : m_Emit(std::make_shared<SolidColor>(color))
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;

    Color3 Emitted(double u, double v, const Math::Point3& p) const override;
    Color3 Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Math::Point3& p) const override;

private:
    std::shared_ptr<Texture> m_Emit;
};

}    // namespace Render
