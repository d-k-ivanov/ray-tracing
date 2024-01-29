#pragma once

#include <Math/Vector3.h>
#include <Render/Hittable.h>
#include <Render/ONB.h>

#include <memory>

class Sphere final : public Hittable
{
public:
    Sphere(const Point3& center, const double radius, const std::shared_ptr<Material>& material)
        : m_Center(center)
        , m_Radius(radius)
        , m_Material(material)
        , m_IsMoving(false)
    {
        const auto rvec = Vector3(radius, radius, radius);
        m_BoundingBox   = AABB(center - rvec, center + rvec);
    }

    Sphere(const Point3& center, const Point3& center2, const double radius, const std::shared_ptr<Material>& material)
        : m_Center(center)
        , m_Radius(radius)
        , m_Material(material)
        , m_IsMoving(true)
    {
        const auto rvec = Vector3(radius, radius, radius);
        const AABB box1(center - rvec, center + rvec);
        const AABB box2(center2 - rvec, center2 + rvec);
        m_BoundingBox = AABB(box1, box2);

        m_CenterV = center - center2;
    }

    bool Hit(const Ray& r, const Interval rayT, HitRecord& rec) const override
    {
        const Point3  center = m_IsMoving ? SphereCenter(r.Time()) : m_Center;
        const Vector3 oc     = center - r.Origin();
        const double  a      = r.Direction().LengthSquared();
        const double  halfB  = DotProduct(r.Direction(), oc);
        const double  c      = oc.LengthSquared() - m_Radius * m_Radius;

        const double discriminant = halfB * halfB - a * c;
        if(discriminant < 0)
        {
            return false;
        }
        const auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (halfB - sqrtd) / a;
        if(!rayT.Surrounds(root))
        {
            root = (halfB + sqrtd) / a;
            if(!rayT.Surrounds(root))
                return false;
        }

        rec.T = root;
        rec.P = r.At(rec.T);

        const Vector3 outwardNormal = (rec.P - m_Center) / m_Radius;
        rec.SetFaceNormal(r, outwardNormal);
        GetSphereUV(outwardNormal, rec.U, rec.V);

        rec.Material = m_Material;

        return true;
    }

    AABB BoundingBox() const override { return m_BoundingBox; }

    // This method only works for stationary spheres.
    double PDFValue(const Point3& o, const Vector3& v) const override
    {
        HitRecord rec;
        if(!this->Hit(Ray(o, v), Interval(0.001, Infinity), rec))
            return 0;

        const auto cosThetaMax = sqrt(1 - m_Radius * m_Radius / (m_Center - o).LengthSquared());
        const auto solidAngle  = 2 * Pi * (1 - cosThetaMax);

        return 1 / solidAngle;
    }

    Vector3 Random(const Point3& o) const override
    {
        const Vector3 direction       = m_Center - o;
        const auto    distanceSquared = direction.LengthSquared();
        ONB           uvw;
        uvw.BuildFromW(direction);
        return uvw.Local(RandomToSphere(m_Radius, distanceSquared));
    }

private:
    Point3                    m_Center;
    Vector3                   m_CenterV;
    double                    m_Radius;
    std::shared_ptr<Material> m_Material;
    bool                      m_IsMoving;
    AABB                      m_BoundingBox;

    // Linearly interpolate from center1 to center2 according to time.
    // Where t=0 yields center1, and t=1 yields center2.
    Point3 SphereCenter(const double time) const
    {
        return m_Center + time * m_CenterV;
    }

    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    static void GetSphereUV(const Point3& p, double& u, double& v)
    {
        const auto theta = acos(-p.Y());
        const auto phi   = atan2(-p.Z(), p.X()) + Pi;

        u = phi / (2 * Pi);
        v = theta / Pi;
    }

    static Vector3 RandomToSphere(const double radius, const double distanceSquared)
    {
        const auto r1 = Random::Double();
        const auto r2 = Random::Double();
        auto       z  = 1 + r2 * (sqrt(1 - radius * radius / distanceSquared) - 1);

        const auto phi = 2 * Pi * r1;
        auto       x   = cos(phi) * sqrt(1 - z * z);
        auto       y   = sin(phi) * sqrt(1 - z * z);

        return {x, y, z};
    }
};
