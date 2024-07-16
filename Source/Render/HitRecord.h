#pragma once

#include <Math/Vector3.h>

#include <memory>

class Material;
class Ray;

class HitRecord
{
public:
    Point3  P;
    Vector3 Normal;
    double  T = 0.0;
    double  U = 0.0;
    double  V = 0.0;
    bool    FrontFace;

    // Sets the hit record normal vector.
    // NOTE: the parameter `outwardNormal` is assumed to have unit length.
    void SetFaceNormal(const Ray& r, const Vector3& outwardNormal);

    std::shared_ptr<Material>& GetMaterial();

    void SetMaterial(const std::shared_ptr<Material>& material);

private:
    std::shared_ptr<Material> m_Material = nullptr;
};
