#pragma once

#include <Math/Vector3.h>

#include <memory>

class Material;
class Ray;

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
    void SetFaceNormal(const Ray& r, const Vector3& outwardNormal);
};
