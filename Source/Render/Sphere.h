#pragma once

#include <Math/Vector3.h>
#include <Render/Hittable.h>

#include <memory>

class Sphere final : public Hittable
{
public:
    Sphere(const Point3& center, const double radius, const std::shared_ptr<Material>& material)
        : m_Center(center)
        , m_Radius(radius)
        , m_Material(material)
    {
    }

    bool Hit(const Ray& r, const Interval rayT, HitRecord& rec) const override
    {
        const Vector3 oc    = r.Origin() - m_Center;
        const double  a     = r.Direction().LengthSquared();
        const double  halfB = DotProduct(oc, r.Direction());
        const double  c     = oc.LengthSquared() - m_Radius * m_Radius;

        const double discriminant = halfB * halfB - a * c;
        if(discriminant < 0)
        {
            return false;
        }
        const auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (-halfB - sqrtd) / a;
        if(!rayT.Surrounds(root))
        {
            root = (-halfB + sqrtd) / a;
            if(!rayT.Surrounds(root))
                return false;
        }

        rec.T                 = root;
        rec.P                 = r.At(rec.T);
        rec.Material          = m_Material;
        Vector3 outwardNormal = (rec.P - m_Center) / m_Radius;
        rec.SetFaceNormal(r, outwardNormal);

        return true;
    }

private:
    Point3                    m_Center;
    double                    m_Radius;
    std::shared_ptr<Material> m_Material;
};
