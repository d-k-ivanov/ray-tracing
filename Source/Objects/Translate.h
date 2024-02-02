#pragma once

#include "Hittable.h"

class Translate final : public Hittable
{
public:
    Translate(const std::shared_ptr<Hittable>& object, const Vector3& displacement);
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    Vector3                   m_Offset;
    AABB                      m_BoundingBox;
};
