#pragma once

#include "Hittable.h"

#include <memory>

namespace Objects
{

class RotateY final : public Hittable
{
public:
    RotateY(const std::shared_ptr<Hittable>& object, double angle);
    bool       Hit(const Render::Ray& ray, Math::Interval rayT, Render::HitRecord& rec) const override;
    Math::AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    double                    m_SinTheta;
    double                    m_CosTheta;
    Math::AABB                m_BoundingBox;
};

}    // namespace Objects
