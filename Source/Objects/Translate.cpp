#include "Translate.h"

#include <Render/HitRecord.h>

namespace Objects
{

Translate::Translate(const std::shared_ptr<Hittable>& object, const Math::Vector3& displacement)
    : m_Object(object)
    , m_Offset(displacement)
{
    m_BoundingBox = m_Object->BoundingBox() + m_Offset;
}

bool Translate::Hit(const Render::Ray& ray, const Math::Interval rayT, Render::HitRecord& rec) const
{
    // Move the ray backwards by the offset
    const Render::Ray offsetRay(ray.Origin() - m_Offset, ray.Direction(), ray.Time());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if(!m_Object->Hit(offsetRay, rayT, rec))
        return false;

    // Move the intersection point forwards by the offset
    rec.P += m_Offset;

    return true;
}

Math::AABB Translate::BoundingBox() const
{
    return m_BoundingBox;
}

}    // namespace Objects
