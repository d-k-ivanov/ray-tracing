#include "Ray.h"

namespace Render
{

const Math::Point3& Ray::Origin() const
{
    return m_Origin;
}

const Math::Vector3& Ray::Direction() const
{
    return m_Direction;
}

double Ray::Time() const
{
    return m_Time;
}

Math::Point3 Ray::At(const double t) const
{
    return m_Origin + t * m_Direction;
}

}    // namespace Render
