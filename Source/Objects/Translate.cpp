#include "Translate.h"

#include <Render/HitRecord.h>

Translate::Translate(const std::shared_ptr<Hittable>& object, const Vector3& displacement)
    : m_Object(object)
    , m_Offset(displacement)
{
    m_BoundingBox = m_Object->BoundingBox() + m_Offset;
}

bool Translate::Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const
{
    // Move the ray backwards by the offset
    const Ray offsetRay(ray.Origin() - m_Offset, ray.Direction(), ray.Time());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if(!m_Object->Hit(offsetRay, rayT, rec))
        return false;

    // Move the intersection point forwards by the offset
    rec.P += m_Offset;

    return true;
}

AABB Translate::BoundingBox() const
{
    return m_BoundingBox;
}
