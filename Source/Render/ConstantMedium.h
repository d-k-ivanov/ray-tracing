#pragma once

#include <Math/Interval.h>
#include <Render/Hittable.h>
#include <Render/Material.h>
#include <Render/Texture.h>

class ConstantMedium final : public Hittable
{
public:
    ConstantMedium(const std::shared_ptr<Hittable>& b, const double d, const std::shared_ptr<Texture>& a)
        : m_Boundary(b)
        , m_NegativeInvertedDensity(-1 / d)
        , m_PhaseFunction(std::make_shared<Isotropic>(a))
    {
    }

    ConstantMedium(const std::shared_ptr<Hittable>& b, const double d, Color3 c)
        : m_Boundary(b)
        , m_NegativeInvertedDensity(-1 / d)
        , m_PhaseFunction(std::make_shared<Isotropic>(c))
    {
    }

    bool Hit(const Ray& ray, const Interval rayT, HitRecord& rec) const override
    {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        constexpr bool enableDebug = false;
        constexpr bool debugging   = enableDebug && Random::Double() < 0.00001;

        HitRecord rec1, rec2;

        if(!m_Boundary->Hit(ray, Interval::Universe, rec1))
        {
            return false;
        }

        if(!m_Boundary->Hit(ray, Interval(rec1.T + 0.0001, Infinity), rec2))
        {
            return false;
        }

        if(debugging)
        {
            std::clog << "\nt_min=" << rec1.T << ", t_max=" << rec2.T << '\n';
        }

        if(rec1.T < rayT.Min)
        {
            rec1.T = rayT.Min;
        }

        if(rec2.T > rayT.Max)
        {
            rec2.T = rayT.Max;
        }

        if(rec1.T >= rec2.T)
        {
            return false;
        }

        if(rec1.T < 0)
        {
            rec1.T = 0;
        }

        const auto rayLength              = ray.Direction().Length();
        const auto distanceInsideBoundary = (rec2.T - rec1.T) * rayLength;
        const auto hitDistance            = m_NegativeInvertedDensity * std::log(Random::Double());

        if(hitDistance > distanceInsideBoundary)
        {
            return false;
        }

        rec.T = rec1.T + hitDistance / rayLength;
        rec.P = ray.At(rec.T);

        if(debugging)
        {
            std::clog << "hit_distance = " << hitDistance << '\n'
                      << "rec.T = " << rec.T << '\n'
                      << "rec.P = " << rec.P << '\n';
        }

        rec.Normal    = Vector3(1, 0, 0);    // arbitrary
        rec.FrontFace = true;                // also arbitrary
        rec.Material  = m_PhaseFunction;

        return true;
    }

    AABB BoundingBox() const override { return m_Boundary->BoundingBox(); }

private:
    std::shared_ptr<Hittable> m_Boundary;
    double                    m_NegativeInvertedDensity;
    std::shared_ptr<Material> m_PhaseFunction;
};
