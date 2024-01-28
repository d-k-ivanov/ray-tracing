// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
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

    void SetFaceNormal(const Ray& r, Vector3& outwardNormal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outwardNormal` is assumed to have unit length.
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
    virtual AABB BoundingBox() const                                    = 0;
};
