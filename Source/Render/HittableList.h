#pragma once

#include <Render/Hittable.h>

#include <memory>
#include <vector>

class HittableList final : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() = default;

    explicit HittableList(const std::shared_ptr<Hittable>& object)
    {
        Add(object);
    }

    void Clear()
    {
        objects.clear();
    }

    void Add(const std::shared_ptr<Hittable>& object)
    {
        objects.emplace_back(object);
    }

    // bool hit(const Ray &r, double ray_tmin, double ray_tmax, HitRecord &rec) const override
    bool Hit(const Ray& r, const Interval rayT, HitRecord& rec) const override
    {
        HitRecord tempRec;
        bool      hitAnything  = false;
        auto      closestSoFar = rayT.Max;

        for(const auto& object : objects)
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
};
