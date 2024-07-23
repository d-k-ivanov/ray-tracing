#pragma once

#include "Hittable.h"

#include <memory>

namespace Render
{
class Material;
}

namespace Objects
{

class Quad final : public Hittable
{
public:
    Quad(const Math::Point3& q, const Math::Vector3& u, const Math::Vector3& v, const std::shared_ptr<Render::Material>& m);

    void       SetBoundingBox();
    Math::AABB BoundingBox() const override;
    bool       Hit(const Render::Ray& ray, Math::Interval rayT, Render::HitRecord& rec) const override;

    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.
    bool IsInterior(double a, double b, Render::HitRecord& rec) const;

    double        PDFValue(const Math::Point3& origin, const Math::Vector3& direction) const override;
    Math::Vector3 Random(const Math::Point3& origin) const override;

private:
    Math::Point3                      m_Q;
    Math::Vector3                     m_U;
    Math::Vector3                     m_V;
    std::shared_ptr<Render::Material> m_Material;
    Math::AABB                        m_BoundingBox;
    Math::Vector3                     m_Normal;
    double                            m_D;
    Math::Vector3                     m_W;
    double                            m_Area;
};

}    // namespace Objects
