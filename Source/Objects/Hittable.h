#pragma once

#include <Math/AABB.h>
#include <Math/Vector3.h>

namespace Math
{
class Interval;
}    // namespace Math

namespace Render
{
class HitRecord;
class Ray;
}    // namespace Math

namespace Objects
{

class Hittable
{
public:
    virtual ~Hittable() = default;

    // Without Interval class:
    // virtual bool hit(const Render::Ray &r, double ray_tmin, double ray_tmax, Render::HitRecord &rec) const = 0;
    virtual bool          Hit(const Render::Ray& r, Math::Interval rayT, Render::HitRecord& rec) const = 0;
    virtual Math::AABB    BoundingBox() const                                                          = 0;
    virtual double        PDFValue(const Math::Vector3& origin, const Math::Vector3& direction) const;
    virtual Math::Vector3 Random(const Math::Vector3& origin) const;
};

}    // namespace Objects
