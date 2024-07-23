#include "SolidColor.h"

namespace Render
{

Color3 SolidColor::Value(double u, double v, const Math::Point3& point) const
{
    return m_ColorValue;
}

}    // namespace Render
