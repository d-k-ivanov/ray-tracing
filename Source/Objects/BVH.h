#pragma once

#include "Hittable.h"
#include "HittableList.h"

#include <memory>

class BVHNode final : public Hittable
{
public:
    explicit BVHNode(const HittableList& list)
        : BVHNode(list.Objects, 0, list.Objects.size())
    {
    }

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end);
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Left;
    std::shared_ptr<Hittable> m_Right;
    AABB                      m_BoundingBox;

    static bool BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int axisIndex);
    static bool BoxXCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
    static bool BoxYCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
    static bool BoxZCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b);
};
