#pragma once

#include "Hittable.h"

class Sphere final : public Hittable
{
public:
    // Stationary Sphere
    Sphere(const Point3& center, double radius, const std::shared_ptr<Material>& material);

    // Moving Sphere
    Sphere(const Point3& center, const Point3& center2, double radius, const std::shared_ptr<Material>& material);

    bool Hit(const Ray& r, Interval rayT, HitRecord& rec) const override;

    AABB BoundingBox() const override;

    // This method only works for stationary spheres.
    double PDFValue(const Point3& o, const Vector3& v) const override;

    Vector3 Random(const Point3& o) const override;

private:
    Point3                    m_Center;
    Vector3                   m_CenterV;
    double                    m_Radius;
    std::shared_ptr<Material> m_Material;
    bool                      m_IsMoving;
    AABB                      m_BoundingBox;

    // Linearly interpolate from center1 to center2 according to time.
    // Where t=0 yields center1, and t=1 yields center2.
    Point3 SphereCenter(double time) const;

    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    static void GetSphereUV(const Point3& p, double& u, double& v);

    static Vector3 RandomToSphere(double radius, double distanceSquared);
};
