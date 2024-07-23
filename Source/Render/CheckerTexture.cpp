#include "CheckerTexture.h"

namespace Render
{

Color3 CheckerTexture::Value(const double u, const double v, const Math::Point3& point) const
{
    const auto xInteger = static_cast<int>(std::floor(m_InvertedScale * point.X()));
    const auto yInteger = static_cast<int>(std::floor(m_InvertedScale * point.Y()));
    const auto zInteger = static_cast<int>(std::floor(m_InvertedScale * point.Z()));

    const bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? m_Even->Value(u, v, point) : m_Odd->Value(u, v, point);
}

}    // namespace Render
