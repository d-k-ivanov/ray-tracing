#pragma once

#include <Render/Hittable.h>
#include <Render/HittableList.h>

#include <algorithm>
#include <memory>

class BVHNode final : public Hittable
{
public:
    explicit BVHNode(const HittableList& list)
        : BVHNode(list.Objects, 0, list.Objects.size())
    {
    }

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end)
    {
        m_BoundingBox = AABB::Empty;
        for(size_t objectIndex = start; objectIndex < end; objectIndex++)
        {
            m_BoundingBox = AABB(m_BoundingBox, srcObjects[objectIndex]->BoundingBox());
        }

        // const int  axis       = Random::Int(0, 2);
        const int  axis       = m_BoundingBox.LongestAxis();
        const auto comparator = (axis == 0) ? BoxXCompare : ((axis == 1) ? BoxYCompare : BoxZCompare);

        // Create a modifiable array of the source scene objects
        auto objects = srcObjects;

        const size_t objectSpan = end - start;

        if(objectSpan == 1)
        {
            m_Left = m_Right = objects[start];
        }
        else if(objectSpan == 2)
        {
            if(comparator(objects[start], objects[start + 1]))
            {
                m_Left  = objects[start];
                m_Right = objects[start + 1];
            }
            else
            {
                m_Left  = objects[start + 1];
                m_Right = objects[start];
            }
        }
        else
        {
            std::sort(objects.begin() + static_cast<long long>(start), objects.begin() + static_cast<long long>(end), comparator);

            auto mid = start + objectSpan / 2;
            m_Left   = std::make_shared<BVHNode>(objects, start, mid);
            m_Right  = std::make_shared<BVHNode>(objects, mid, end);
        }

        // m_BoundingBox = AABB(m_Left->BoundingBox(), m_Right->BoundingBox());
    }

    bool Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const override
    {
        if(!m_BoundingBox.Hit(ray, rayT))
            return false;

        const bool hitLeft  = m_Left->Hit(ray, rayT, rec);
        const bool hitRight = m_Right->Hit(ray, Interval(rayT.Min, hitLeft ? rec.T : rayT.Max), rec);

        return hitLeft || hitRight;
    }

    AABB BoundingBox() const override { return m_BoundingBox; }

private:
    std::shared_ptr<Hittable> m_Left;
    std::shared_ptr<Hittable> m_Right;
    AABB                      m_BoundingBox;

    static bool BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, const int axisIndex)
    {
        return a->BoundingBox().Axis(axisIndex).Min < b->BoundingBox().Axis(axisIndex).Min;
    }

    static bool BoxXCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
    {
        return BoxCompare(a, b, 0);
    }

    static bool BoxYCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
    {
        return BoxCompare(a, b, 1);
    }

    static bool BoxZCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
    {
        return BoxCompare(a, b, 2);
    }
};
