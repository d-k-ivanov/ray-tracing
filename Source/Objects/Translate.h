#pragma once

#include "Hittable.h"

#include <memory>

namespace Objects
{

class Translate final : public Hittable
{
public:
    Translate(const std::shared_ptr<Hittable>& object, const Math::Vector3& displacement);
    bool       Hit(const Render::Ray& ray, Math::Interval rayT, Render::HitRecord& rec) const override;
    Math::AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    Math::Vector3             m_Offset;
    Math::AABB                m_BoundingBox;
};

}    // namespace Objects
