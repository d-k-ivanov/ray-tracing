#include "NoiseTextureCamouflage.h"

namespace Render
{

Color3 NoiseTextureCamouflage::Value(double u, double v, const Math::Point3& p) const
{
    const auto s = m_Scale * p;
    return Color3(1, 1, 1) * m_Noise.Turbulence(s, 7);
}

}    // namespace Render
