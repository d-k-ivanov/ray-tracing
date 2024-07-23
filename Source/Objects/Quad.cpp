#include "Quad.h"

#include <Render/HitRecord.h>

namespace Objects
{

Quad::Quad(const Math::Point3& q, const Math::Vector3& u, const Math::Vector3& v, const std::shared_ptr<Render::Material>& m)
    : m_Q(q)
    , m_U(u)
    , m_V(v)
    , m_Material(m)
{
    const auto n = CrossProduct(m_U, m_V);
    m_Normal     = UnitVector(n);
    m_D          = DotProduct(m_Normal, m_Q);
    m_W          = n / DotProduct(n, n);
    m_Area       = n.Length();

    SetBoundingBox();
}

void Quad::SetBoundingBox()
{
    const Math::AABB bbox1 = Math::AABB(m_Q, m_Q + m_U + m_V);
    const Math::AABB bbox2 = Math::AABB(m_Q + m_U, m_Q + m_V);
    m_BoundingBox          = Math::AABB(bbox1, bbox2);
}

Math::AABB Quad::BoundingBox() const
{
    return m_BoundingBox;
}

bool Quad::Hit(const Render::Ray& ray, const Math::Interval rayT, Render::HitRecord& rec) const
{
    const auto denom = DotProduct(m_Normal, ray.Direction());

    // No hit if the ray is parallel to the plane.
    if(fabs(denom) < 1e-8)
        return false;

    // Return false if the hit point parameter t is outside the ray interval.
    const auto t = (m_D - DotProduct(m_Normal, ray.Origin())) / denom;
    if(!rayT.Contains(t))
        return false;

    // Determine the hit point lies within the planar shape using its plane coordinates.
    const auto          intersection      = ray.At(t);
    const Math::Vector3 planarHitptVector = intersection - m_Q;
    const auto          alpha             = DotProduct(m_W, CrossProduct(planarHitptVector, m_V));
    const auto          beta              = DotProduct(m_W, CrossProduct(m_U, planarHitptVector));

    if(!IsInterior(alpha, beta, rec))
        return false;

    // Ray hits the 2D shape; set the rest of the hit record and return true.
    rec.T = t;
    rec.P = intersection;
    rec.SetMaterial(m_Material);
    rec.SetFaceNormal(ray, m_Normal);

    return true;
}

// Given the hit point in plane coordinates, return false if it is outside the
// primitive, otherwise set the hit record UV coordinates and return true.
bool Quad::IsInterior(const double a, const double b, Render::HitRecord& rec) const
{
    if((a < 0) || (1 < a) || (b < 0) || (1 < b))
        return false;

    rec.U = a;
    rec.V = b;
    return true;
}

double Quad::PDFValue(const Math::Point3& origin, const Math::Vector3& direction) const
{
    Render::HitRecord rec;
    if(!this->Hit(Render::Ray(origin, direction), Math::Interval(0.001, Math::Infinity), rec))
        return 0;

    const auto distanceSquared = rec.T * rec.T * direction.LengthSquared();

    const auto cosine = std::fabs(DotProduct(direction, rec.Normal) / direction.Length());

    return distanceSquared / (cosine * m_Area);
}

Math::Vector3 Quad::Random(const Math::Point3& origin) const
{
    const auto p = m_Q + (Utils::Random::Double() * m_U) + (Utils::Random::Double() * m_V);
    return p - origin;
}

}    // namespace Objects
