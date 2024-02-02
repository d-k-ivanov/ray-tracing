#include "Ray.h"

const Point3& Ray::Origin() const
{
    return m_Origin;
}

const Vector3& Ray::Direction() const
{
    return m_Direction;
}

double Ray::Time() const
{
    return m_Time;
}

Point3 Ray::At(const double t) const
{
    return m_Origin + t * m_Direction;
}
