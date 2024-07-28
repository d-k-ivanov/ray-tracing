#include "HittableList.h"

#include <Render/HitRecord.h>

namespace Objects
{

HittableList::HittableList(const std::shared_ptr<Hittable>& object)
{
    Add(object);
}

void HittableList::Clear()
{
    Objects.clear();
}

void HittableList::Add(const std::shared_ptr<Hittable>& object)
{
    Objects.emplace_back(object);
    m_BoundingBox = Math::AABB(m_BoundingBox, object->BoundingBox());
}

bool HittableList::Hit(const Render::Ray& r, const Math::Interval rayT, Render::HitRecord& rec) const
{
    Render::HitRecord tempRec;
    bool              hitAnything  = false;
    auto              closestSoFar = rayT.Max;

    for(const auto& object : Objects)
    {
        if(object->Hit(r, Math::Interval(rayT.Min, closestSoFar), tempRec))
        {
            hitAnything  = true;
            closestSoFar = tempRec.T;
            rec          = tempRec;
        }
    }

    return hitAnything;
}

Math::AABB HittableList::BoundingBox() const
{
    return m_BoundingBox;
}

double HittableList::PDFValue(const Math::Point3& origin, const Math::Vector3& direction) const
{
    const auto weight = 1.0 / static_cast<double>(Objects.size());
    auto       sum    = 0.0;

    for(const auto& object : Objects)
        sum += weight * object->PDFValue(origin, direction);

    return sum;
}

Math::Vector3 HittableList::Random(const Math::Vector3& origin) const
{
    const auto intSize = static_cast<int>(Objects.size());
    return Objects[Utils::Random::Int(0, intSize - 1)]->Random(origin);
}

}    // namespace Objects
