#include "NoiseTextureSmooth.h"

namespace Render
{

Color3 NoiseTextureSmooth::Value(double u, double v, const Math::Point3& p) const
{
    return Color3(1, 1, 1) * 0.5 * (1.0 + m_Noise.Noise(m_Scale * p));
}

}    // namespace Render
