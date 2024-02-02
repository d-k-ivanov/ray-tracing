#include "SolidColor.h"

Color3 SolidColor::Value(double u, double v, const Point3& point) const
{
    return m_ColorValue;
}