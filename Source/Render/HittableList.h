#pragma once

#include <Render/Hittable.h>

#include <memory>
#include <vector>

class HittableList final : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> Objects;

    HittableList() = default;

    explicit HittableList(const std::shared_ptr<Hittable>& object)
    {
        Add(object);
    }

    void Clear()
    {
        Objects.clear();
    }

    void Add(const std::shared_ptr<Hittable>& object)
    {
        Objects.emplace_back(object);
        m_BoundingBox = AABB(m_BoundingBox, object->BoundingBox());
    }

    // bool hit(const Ray &r, double ray_tmin, double ray_tmax, HitRecord &rec) const override
    bool Hit(const Ray& r, const Interval rayT, HitRecord& rec) const override
    {
        HitRecord tempRec;
        bool      hitAnything  = false;
        auto      closestSoFar = rayT.Max;

        for(const auto& object : Objects)
        {
            if(object->Hit(r, Interval(rayT.Min, closestSoFar), tempRec))
            {
                hitAnything  = true;
                closestSoFar = tempRec.T;
                rec          = tempRec;
            }
        }

        return hitAnything;
    }

    AABB BoundingBox() const override { return m_BoundingBox; }

    double PDFValue(const Point3& origin, const Vector3& direction) const override
    {
        const auto weight = 1.0 / static_cast<double>(Objects.size());
        auto       sum    = 0.0;

        for(const auto& object : Objects)
            sum += weight * object->PDFValue(origin, direction);

        return sum;
    }

    Vector3 Random(const Vector3& origin) const override
    {
        const auto intSize = static_cast<int>(Objects.size());
        return Objects[Random::Int(0, intSize - 1)]->Random(origin);
    }

private:
    AABB m_BoundingBox;
};
