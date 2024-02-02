#pragma once

#include "Hittable.h"

class Material;

class Quad final : public Hittable
{
public:
    Quad(const Point3& q, const Vector3& u, const Vector3& v, const std::shared_ptr<Material>& m);

    void SetBoundingBox();
    AABB BoundingBox() const override;
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;

    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.
    bool IsInterior(double a, double b, HitRecord& rec) const;

    double  PDFValue(const Point3& origin, const Vector3& direction) const override;
    Vector3 Random(const Point3& origin) const override;

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
