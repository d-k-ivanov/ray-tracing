#pragma once

#include "Hittable.h"

#include <memory>

class RotateY final : public Hittable
{
public:
    RotateY(const std::shared_ptr<Hittable>& object, double angle);
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    double                    m_SinTheta;
    double                    m_CosTheta;
    AABB                      m_BoundingBox;
};
