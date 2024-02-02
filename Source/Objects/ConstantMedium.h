#pragma once

#include "Hittable.h"

#include <Render/Material.h>

class ConstantMedium final : public Hittable
{
public:
    ConstantMedium(const std::shared_ptr<Hittable>& boundary, const double density, const std::shared_ptr<Texture>& texture)
        : m_Boundary(boundary)
        , m_NegativeInvertedDensity(-1 / density)
        , m_PhaseFunction(std::make_shared<Isotropic>(texture))
    {
    }

    ConstantMedium(const std::shared_ptr<Hittable>& boundary, const double density, const Color3& color)
        : m_Boundary(boundary)
        , m_NegativeInvertedDensity(-1 / density)
        , m_PhaseFunction(std::make_shared<Isotropic>(color))
    {
    }

    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Boundary;
    double                    m_NegativeInvertedDensity;
    std::shared_ptr<Material> m_PhaseFunction;
};
