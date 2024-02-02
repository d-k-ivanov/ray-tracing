#include "HittableList.h"

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
    m_BoundingBox = AABB(m_BoundingBox, object->BoundingBox());
}

bool HittableList::Hit(const Ray& r, const Interval rayT, HitRecord& rec) const
{
    HitRecord tempRec;
    bool      hitAnything  = false;
    auto      closestSoFar = rayT.Max;

    for(const auto& object : Objects)
    {
        if(object->Hit(r, Interval(rayT.Min, closestSoFar), tempRec))
        {
            hitAnything  = true;
            closestSoFar = tempRec.T;
            rec          = tempRec;
        }
    }

    return hitAnything;
}

AABB HittableList::BoundingBox() const
{
    return m_BoundingBox;
}

double HittableList::PDFValue(const Point3& origin, const Vector3& direction) const
{
    const auto weight = 1.0 / static_cast<double>(Objects.size());
    auto       sum    = 0.0;

    for(const auto& object : Objects)
        sum += weight * object->PDFValue(origin, direction);

    return sum;
}

Vector3 HittableList::Random(const Vector3& origin) const
{
    const auto intSize = static_cast<int>(Objects.size());
    return Objects[Random::Int(0, intSize - 1)]->Random(origin);
}
