#include "NoiseTexture.h"

namespace Render
{

Color3 NoiseTexture::Value(double u, double v, const Math::Point3& p) const
{
    const auto s = m_Scale * p;
    return Color3(1, 1, 1) * 0.5 * (1 + sin(s.Z() + 10 * m_Noise.Turbulence(s, 7)));
}

}    // namespace Render
