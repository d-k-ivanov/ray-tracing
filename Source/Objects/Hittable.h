#pragma once

#include <Math/AABB.h>
#include <Math/Interval.h>
#include <Math/Vector3.h>
#include <Render/HitRecord.h>
#include <Render/Ray.h>

#include <memory>

class Hittable
{
public:
    virtual ~Hittable() = default;

    // old:
    // virtual bool hit(const Ray &r, double ray_tmin, double ray_tmax, HitRecord &rec) const = 0;
    virtual bool    Hit(const Ray& r, Interval rayT, HitRecord& rec) const = 0;
    virtual AABB    BoundingBox() const                                    = 0;
    virtual double  PDFValue(const Vector3& origin, const Vector3& direction) const;
    virtual Vector3 Random(const Vector3& origin) const;
};

class Translate final : public Hittable
{
public:
    Translate(const std::shared_ptr<Hittable>& object, const Vector3& displacement);
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    Vector3                   m_Offset;
    AABB                      m_BoundingBox;
};

class RotateY final : public Hittable
{
public:
    RotateY(const std::shared_ptr<Hittable>& object, double angle);
    bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB BoundingBox() const override;

private:
    std::shared_ptr<Hittable> m_Object;
    double                    m_SinTheta;
    double                    m_CosTheta;
    AABB                      m_BoundingBox;
};
