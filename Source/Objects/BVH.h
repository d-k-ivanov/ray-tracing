#pragma once

#include "Hittable.h"
#include "HittableList.h"

namespace Objects
{

class BVHNode final : public Hittable
{
public:
    explicit BVHNode(const HittableList& list)
        : BVHNode(list.Objects)
    {
    }

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects);
    bool       Hit(const Render::Ray& ray, Math::Interval rayT, Render::HitRecord& rec) const override;
    Math::AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Left;
    std::shared_ptr<Hittable> m_Right;
    Math::AABB                m_BoundingBox;

    static bool BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int axisIndex);
    static bool BoxXCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
    static bool BoxYCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
    static bool BoxZCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
};

}    // namespace Math
