#pragma once

#include <Math/Interval.h>
#include <Math/Vector3.h>
#include <Render/AABB.h>
#include <Render/Ray.h>

#include <memory>

class Material;

class HitRecord
{
public:
    Point3                    P;
    Vector3                   Normal;
    std::shared_ptr<Material> Material;
    double                    T;
    double                    U;
    double                    V;
    bool                      FrontFace;

    // Sets the hit record normal vector.
    // NOTE: the parameter `outwardNormal` is assumed to have unit length.
    void SetFaceNormal(const Ray& r, const Vector3& outwardNormal)
    {
        FrontFace = DotProduct(r.Direction(), outwardNormal) < 0;
        Normal    = FrontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    // old:
    // virtual bool hit(const Ray &r, double ray_tmin, double ray_tmax, HitRecord &rec) const = 0;
    virtual bool Hit(const Ray& r, Interval rayT, HitRecord& rec) const = 0;

    virtual AABB BoundingBox() const = 0;

    virtual double PDFValue(const Vector3& origin, const Vector3& direction) const
    {
        return 0.0;
    }

    virtual Vector3 Random(const Vector3& origin) const
    {
        return {1, 0, 0};
    }
};

class Translate final : public Hittable
{
public:
    Translate(const std::shared_ptr<Hittable>& object, const Vector3& displacement)
        : m_Object(object)
        , m_Offset(displacement)
    {
        m_BoundingBox = m_Object->BoundingBox() + m_Offset;
    }

    bool Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const override
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

    AABB BoundingBox() const override { return m_BoundingBox; }

private:
    std::shared_ptr<Hittable> m_Object;
    Vector3                   m_Offset;
    AABB                      m_BoundingBox;
};

class RotateY final : public Hittable
{
public:
    RotateY(const std::shared_ptr<Hittable>& object, const double angle)
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

    bool Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const override
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

    AABB BoundingBox() const override { return m_BoundingBox; }

private:
    std::shared_ptr<Hittable> m_Object;
    double                    m_SinTheta;
    double                    m_CosTheta;
    AABB                      m_BoundingBox;
};
