#pragma once

#include "Hittable.h"

class HittableList final : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> Objects;

    HittableList() = default;

    explicit HittableList(const std::shared_ptr<Hittable>& object);

    void    Clear();
    void    Add(const std::shared_ptr<Hittable>& object);
    bool    Hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
    AABB    BoundingBox() const override;
    double  PDFValue(const Point3& origin, const Vector3& direction) const override;
    Vector3 Random(const Vector3& origin) const override;

private:
    AABB m_BoundingBox;
};
