#pragma once

#include <Render/Hittable.h>
#include <Render/HittableList.h>

#include <memory>

class Quad final : public Hittable
{
public:
    Quad(const Point3& q, const Vector3& u, const Vector3& v, const std::shared_ptr<Material>& m)
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

    void SetBoundingBox()
    {
        m_BoundingBox = AABB(m_Q, m_Q + m_U + m_V);
    }

    AABB BoundingBox() const override { return m_BoundingBox; }

    bool Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const override
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
        const auto    intersection      = ray.At(t);
        const Vector3 planarHitptVector = intersection - m_Q;
        const auto    alpha             = DotProduct(m_W, CrossProduct(planarHitptVector, m_V));
        const auto    beta              = DotProduct(m_W, CrossProduct(m_U, planarHitptVector));

        if(!IsInterior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.T        = t;
        rec.P        = intersection;
        rec.Material = m_Material;
        rec.SetFaceNormal(ray, m_Normal);

        return true;
    }

    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.
    bool IsInterior(const double a, const double b, HitRecord& rec) const
    {
        if((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.U = a;
        rec.V = b;
        return true;
    }

    double PDFValue(const Point3& origin, const Vector3& direction) const override
    {
        HitRecord rec;
        if(!this->Hit(Ray(origin, direction), Interval(0.001, Infinity), rec))
            return 0;

        const auto distanceSquared = rec.T * rec.T * direction.LengthSquared();

        const auto cosine = std::fabs(DotProduct(direction, rec.Normal) / direction.Length());

        return distanceSquared / (cosine * m_Area);
    }

    Vector3 Random(const Point3& origin) const override
    {
        const auto p = m_Q + (Random::Double() * m_U) + (Random::Double() * m_V);
        return p - origin;
    }

private:
    Point3                    m_Q;
    Vector3                   m_U;
    Vector3                   m_V;
    std::shared_ptr<Material> m_Material;
    AABB                      m_BoundingBox;
    Vector3                   m_Normal;
    double                    m_D;
    Vector3                   m_W;
    double                    m_Area;
};

inline std::shared_ptr<HittableList> Box(const Point3& a, const Point3& b, const std::shared_ptr<Material>& material)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = std::make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    const auto min = Point3(fmin(a.X(), b.X()), fmin(a.Y(), b.Y()), fmin(a.Z(), b.Z()));
    const auto max = Point3(fmax(a.X(), b.X()), fmax(a.Y(), b.Y()), fmax(a.Z(), b.Z()));

    auto dx = Vector3(max.X() - min.X(), 0, 0);
    auto dy = Vector3(0, max.Y() - min.Y(), 0);
    auto dz = Vector3(0, 0, max.Z() - min.Z());

    sides->Add(std::make_shared<Quad>(Point3(min.X(), min.Y(), max.Z()), dx, dy, material));     // front
    sides->Add(std::make_shared<Quad>(Point3(max.X(), min.Y(), max.Z()), -dz, dy, material));    // right
    sides->Add(std::make_shared<Quad>(Point3(max.X(), min.Y(), min.Z()), -dx, dy, material));    // back
    sides->Add(std::make_shared<Quad>(Point3(min.X(), min.Y(), min.Z()), dz, dy, material));     // left
    sides->Add(std::make_shared<Quad>(Point3(min.X(), max.Y(), max.Z()), dx, -dz, material));    // top
    sides->Add(std::make_shared<Quad>(Point3(min.X(), min.Y(), min.Z()), dx, dz, material));     // bottom

    return sides;
}
