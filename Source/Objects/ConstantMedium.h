#pragma once

#include "Hittable.h"

#include <Render/Isotropic.h>

namespace Objects
{

class ConstantMedium final : public Hittable
{
public:
    ConstantMedium(const std::shared_ptr<Hittable>& boundary, const double density, const std::shared_ptr<Render::Texture>& texture)
        : m_Boundary(boundary)
        , m_NegativeInvertedDensity(-1 / density)
        , m_PhaseFunction(std::make_shared<Render::Isotropic>(texture))
    {
    }

    ConstantMedium(const std::shared_ptr<Hittable>& boundary, const double density, const Render::Color3& color)
        : m_Boundary(boundary)
        , m_NegativeInvertedDensity(-1 / density)
        , m_PhaseFunction(std::make_shared<Render::Isotropic>(color))
    {
    }

    bool       Hit(const Render::Ray& ray, Math::Interval rayT, Render::HitRecord& rec) const override;
    Math::AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable>         m_Boundary;
    double                            m_NegativeInvertedDensity;
    std::shared_ptr<Render::Material> m_PhaseFunction;
};

}    // namespace Objects
