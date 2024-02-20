// ReSharper disable CppUseRangeAlgorithm
#include "BVH.h"

#include <Render/HitRecord.h>

#include <algorithm>
#include <execution>

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects)
{
    m_BoundingBox = AABB::Empty;
    const size_t size = srcObjects.size();
    for(size_t objectIndex = 0; objectIndex < size; objectIndex++)
    {
        m_BoundingBox = AABB(m_BoundingBox, srcObjects[objectIndex]->BoundingBox());
    }

    // const int  axis       = Random::Int(0, 2);
    const int  axis       = m_BoundingBox.LongestAxis();
    const auto comparator = (axis == 0) ? BoxXCompare : ((axis == 1) ? BoxYCompare : BoxZCompare);

    // Create a modifiable array of the source scene objects
    auto objects = srcObjects;

    if(size == 1)
    {
        m_Left = m_Right = objects[0];
    }
    else if(size == 2)
    {
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
    return a->BoundingBox().Axis(axisIndex).Min < b->BoundingBox().Axis(axisIndex).Min;
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
