#include "Hittable.h"

#include <Math/AABB.h>
#include <Math/Converters.h>
#include <Math/Interval.h>
#include <Math/Vector3.h>
#include <Render/Ray.h>

#include <memory>

double Hittable::PDFValue(const Vector3& origin, const Vector3& direction) const
{
    return 0.0;
}

Vector3 Hittable::Random(const Vector3& origin) const
{
    return {1, 0, 0};
}

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

RotateY::RotateY(const std::shared_ptr<Hittable>& object, const double angle)
    : m_Object(object)
{
    const auto radians = DegreesToRadians(angle);
    m_SinTheta         = std::sin(radians);
    m_CosTheta         = std::cos(radians);
    m_BoundingBox      = m_Object->BoundingBox();

    Point3 min(Infinity, Infinity, Infinity);
    Point3 max(-Infinity, -Infinity, -Infinity);

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                const auto x = i * m_BoundingBox.X.Max + (1 - i) * m_BoundingBox.X.Min;
                const auto y = j * m_BoundingBox.Y.Max + (1 - j) * m_BoundingBox.Y.Min;
                const auto z = k * m_BoundingBox.Z.Max + (1 - k) * m_BoundingBox.Z.Min;

                const auto newx = m_CosTheta * x + m_SinTheta * z;
                const auto newz = -m_SinTheta * x + m_CosTheta * z;

                Vector3 tester(newx, y, newz);

                for(int c = 0; c < 3; c++)
                {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    m_BoundingBox = AABB(min, max);
}

bool RotateY::Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const
{
    // Change the ray from world space to object space
    auto origin    = ray.Origin();
    auto direction = ray.Direction();

    origin[0] = m_CosTheta * ray.Origin()[0] - m_SinTheta * ray.Origin()[2];
    origin[2] = m_SinTheta * ray.Origin()[0] + m_CosTheta * ray.Origin()[2];

    direction[0] = m_CosTheta * ray.Direction()[0] - m_SinTheta * ray.Direction()[2];
    direction[2] = m_SinTheta * ray.Direction()[0] + m_CosTheta * ray.Direction()[2];

    const Ray rotatedRray(origin, direction, ray.Time());

    // Determine whether an intersection exists in object space (and if so, where)
    if(!m_Object->Hit(rotatedRray, rayT, rec))
        return false;

    // Change the intersection point from object space to world space
    auto p = rec.P;
    p[0]   = m_CosTheta * rec.P[0] + m_SinTheta * rec.P[2];
    p[2]   = -m_SinTheta * rec.P[0] + m_CosTheta * rec.P[2];

    // Change the normal from object space to world space
    auto normal = rec.Normal;
    normal[0]   = m_CosTheta * rec.Normal[0] + m_SinTheta * rec.Normal[2];
    normal[2]   = -m_SinTheta * rec.Normal[0] + m_CosTheta * rec.Normal[2];

    rec.P      = p;
    rec.Normal = normal;

    return true;
}

AABB RotateY::BoundingBox() const
{
    return m_BoundingBox;
}
