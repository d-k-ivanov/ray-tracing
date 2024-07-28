#pragma once

#include <Math/Vector3.h>

#include <memory>

namespace Render
{

class Material;
class Ray;

class HitRecord
{
public:
    Math::Point3  P;
    Math::Vector3 Normal;
    double        T         = 0.0;
    double        U         = 0.0;
    double        V         = 0.0;
    bool          FrontFace = false;

    // Sets the hit record normal vector.
    // NOTE: the parameter `outwardNormal` is assumed to have unit length.
    void SetFaceNormal(const Ray& r, const Math::Vector3& outwardNormal);

    std::shared_ptr<Material>& GetMaterial();

    void SetMaterial(const std::shared_ptr<Material>& material);

private:
    std::shared_ptr<Material> m_Material = nullptr;
};

}    // namespace Render
