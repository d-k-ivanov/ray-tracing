// ReSharper disable CppUseRangeAlgorithm
#include "BVH.h"

#include <Render/HitRecord.h>

#include <algorithm>
#include <execution>

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects)
{
    m_BoundingBox = AABB::Empty;

    for(const auto& object : srcObjects)
    {
        m_BoundingBox = AABB(m_BoundingBox, object->BoundingBox());
    }

    // const int  axis       = Random::Int(0, 2);
    const int  axis       = m_BoundingBox.LongestAxis();
    const auto comparator = (axis == 0) ? BoxXCompare : ((axis == 1) ? BoxYCompare : BoxZCompare);

    // Create a modifiable array of the source scene objects
    auto         objects = srcObjects;
    const size_t size    = srcObjects.size();

    if(size == 1)
    {
        m_Left = m_Right = objects[0];
    }
    else if(size == 2)
    {
        // Technically, we can arbitrarily divide the objects into two groups, and everything will still work.
        // However, an arbitrary partition will yield bounding volumes that will likely overlap quite a bit.
        //
        // With many objects, ordering them in one dimension will yield a tighter cluster, and thus smaller and
        // minimally overlapping boxes for the two groups. With only two objects, though, dividing is trivial,
        // and the two boxes will be as small as can be. Order doesn't matter, and the code should be simplified
        // by keeping only the first branch to if-statement.
        //
        // I prefer to keep the code as it is because it's more universal and the performance gain is negligible.
        if(comparator(objects[0], objects[1]))
        {
            m_Left  = objects[0];
            m_Right = objects[1];
        }
        else
        {
            m_Left  = objects[1];
            m_Right = objects[0];
        }
    }
    else
    {
        const auto mid = size / 2;

        // The fastest way to create BVH Trees: https://github.com/RayTracing/raytracing.github.io/issues/1388
        std::nth_element(std::execution::par, objects.begin(), objects.begin() + static_cast<long long>(mid), objects.end(), comparator);

        auto objectsLeft  = std::vector(objects.begin(), objects.begin() + static_cast<long long>(mid));
        auto objectsRight = std::vector(objects.begin() + static_cast<long long>(mid), objects.end());

        m_Left  = make_shared<BVHNode>(objectsLeft);
        m_Right = make_shared<BVHNode>(objectsRight);
    }

    // m_BoundingBox = AABB(m_Left->BoundingBox(), m_Right->BoundingBox());
}

bool BVHNode::Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const
{
    if(!m_BoundingBox.Hit(ray, rayT))
        return false;

    const bool hitLeft  = m_Left->Hit(ray, rayT, rec);
    const bool hitRight = m_Right->Hit(ray, Interval(rayT.Min, hitLeft ? rec.T : rayT.Max), rec);

    return hitLeft || hitRight;
}

AABB BVHNode::BoundingBox() const
{
    return m_BoundingBox;
}

bool BVHNode::BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, const int axisIndex)
{
    return a->BoundingBox().AxisInterval(axisIndex).Min < b->BoundingBox().AxisInterval(axisIndex).Min;

    // New approach: https://github.com/RayTracing/raytracing.github.io/pull/1422
    // const auto aAxisInterval = a->BoundingBox().AxisInterval(axisIndex);
    // const auto bAxisInterval = b->BoundingBox().AxisInterval(axisIndex);
    // return aAxisInterval.Min < bAxisInterval.Min;
}

bool BVHNode::BoxXCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
{
    return BoxCompare(a, b, 0);
}

bool BVHNode::BoxYCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
{
    return BoxCompare(a, b, 1);
}

bool BVHNode::BoxZCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
{
    return BoxCompare(a, b, 2);
}
