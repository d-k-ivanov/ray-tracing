#pragma once

#include "Hittable.h"

#include <memory>

namespace Objects
{

class HittableList final : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> Objects;

    HittableList() = default;

    explicit HittableList(const std::shared_ptr<Hittable>& object);

    void          Clear();
    void          Add(const std::shared_ptr<Hittable>& object);
    bool          Hit(const Render::Ray& r, Math::Interval rayT, Render::HitRecord& rec) const override;
    Math::AABB    BoundingBox() const override;
    double        PDFValue(const Math::Point3& origin, const Math::Vector3& direction) const override;
    Math::Vector3 Random(const Math::Vector3& origin) const override;

private:
    Math::AABB m_BoundingBox;
};

}    // namespace Objects
