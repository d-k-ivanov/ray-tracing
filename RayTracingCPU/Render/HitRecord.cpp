#include "HitRecord.h"

#include "Ray.h"

namespace Render
{

void HitRecord::SetFaceNormal(const Ray& r, const Math::Vector3& outwardNormal)
{
    FrontFace = DotProduct(r.Direction(), outwardNormal) < 0;
    Normal    = FrontFace ? outwardNormal : -outwardNormal;
}

std::shared_ptr<Material>& HitRecord::GetMaterial()
{
    return m_Material;
}

void HitRecord::SetMaterial(const std::shared_ptr<Material>& material)
{
    m_Material = material;
}

}    // namespace Render
