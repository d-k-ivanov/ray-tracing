#pragma once

#include <Math/AABB.h>
#include <Math/Vector3.h>

class Interval;
class HitRecord;
class Ray;

class Hittable
{
public:
    virtual ~Hittable() = default;

    // Without Interval class:
    // virtual bool hit(const Ray &r, double ray_tmin, double ray_tmax, HitRecord &rec) const = 0;
    virtual bool    Hit(const Ray& r, Interval rayT, HitRecord& rec) const = 0;
    virtual AABB    BoundingBox() const                                    = 0;
    virtual double  PDFValue(const Vector3& origin, const Vector3& direction) const;
    virtual Vector3 Random(const Vector3& origin) const;
};
