// ReSharper disable CppUnreachableCode
#include "ConstantMedium.h"

#include <Render/HitRecord.h>

namespace Objects
{

#pragma warning(push)
#pragma warning(disable : 6237)    // warning C6237: debugging variables
bool ConstantMedium::Hit(const Render::Ray& ray, const Math::Interval rayT, Render::HitRecord& rec) const
{
    // Print occasional samples when debugging. To enable, set enableDebug true.
    constexpr bool enableDebug = false;
    constexpr bool debugging   = enableDebug && Utils::Random::Double() < 0.00001;

    Render::HitRecord rec1, rec2;

    if(!m_Boundary->Hit(ray, Math::Interval::Universe, rec1))
    {
        return false;
    }

    if(!m_Boundary->Hit(ray, Math::Interval(rec1.T + 0.0001, Math::Infinity), rec2))
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
    const auto hitDistance            = m_NegativeInvertedDensity * std::log(Utils::Random::Double());

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

    rec.Normal    = Math::Vector3(1, 0, 0);    // arbitrary
    rec.FrontFace = true;                      // also arbitrary
    rec.SetMaterial(m_PhaseFunction);

    return true;
}
#pragma warning(pop)

Math::AABB ConstantMedium::BoundingBox() const
{
    return m_Boundary->BoundingBox();
}

}    // namespace Objects
